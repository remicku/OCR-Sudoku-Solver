#include "ui.h"
#include "grayscale.h"
#include "solver.h"
#include "detection.h"
#include "err.h"
#include "string.h"

//global variables
GtkBuilder* builder;

//main window
GtkWindow* window;
GtkImage* image;
SDL_Surface* image_surface;
SDL_Surface* gray_surface;
SDL_Surface* color_surface;
SDL_Surface* new_surface;

GtkFileChooserButton* open_file_button;
gchar* filename;

GtkScale* manual_rotation_scale;
GtkLabel* manual_rotation_label;

GtkButton* quit_button;
GtkButton* preprocess_button;
GtkButton* automatic_rotation_button;
GtkButton* neural_button;
GtkButton* solver_button;

GtkCheckButton* preprocess_cb;

GtkButton* save_button;

Output* output;
int clicked = 0;

int surface_to_gtk_image(GtkImage *image_container, SDL_Surface *surface){
    Uint32 source_format;
    Uint32 destination_format;
    GdkPixbuf *pixbuf;
    gboolean has_alpha;
    int rowstride;
    guchar *pixels;


    // select format                                                            
    source_format = surface->format->format;
    has_alpha = SDL_ISPIXELFORMAT_ALPHA(source_format);
    if (has_alpha) {
        destination_format = SDL_PIXELFORMAT_RGBA32;
    }
    else {
        destination_format = SDL_PIXELFORMAT_RGB24;
    }

    // create pixbuf                                                            
    pixbuf = gdk_pixbuf_new (GDK_COLORSPACE_RGB, has_alpha, 8,
                             surface->w, surface->h);
    rowstride = gdk_pixbuf_get_rowstride (pixbuf);
    pixels = gdk_pixbuf_get_pixels (pixbuf);

    // copy pixels                                                              
    SDL_LockSurface(surface);
    SDL_ConvertPixels (surface->w, surface->h, source_format,
               surface->pixels, surface->pitch,
               destination_format, pixels, rowstride);
    SDL_UnlockSurface(surface);

    float expected_weight = 900;
    float expected_height = 900;
    float newWeight;
    float newHeight;
    float expected_ratio = expected_weight / expected_height;
    float curr_ratio = surface->w / surface->h;
    if (expected_ratio > curr_ratio) {
        newWeight = surface->w * (expected_height / surface->h);
        newHeight = expected_height;
    } else {
        newWeight = expected_height;
        newHeight = surface->h * (expected_weight / surface->w);
    }

    GdkPixbuf *pxbscaled =
        gdk_pixbuf_scale_simple(pixbuf, newWeight, newHeight, GDK_INTERP_BILINEAR);

    gtk_image_set_from_pixbuf(image_container, pxbscaled);
    
    g_object_unref(pixbuf);
    g_object_unref(pxbscaled);
    return 1;
}

gchar* get_image(){
    image_surface = load_image(gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(open_file_button)));
    color_surface = image_surface;
    filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(open_file_button));
    surface_to_gtk_image(image, image_surface);
    if (filename != NULL){
        gtk_range_set_range(GTK_RANGE(manual_rotation_scale), -180, 180);
        gtk_range_set_value(GTK_RANGE(manual_rotation_scale), 0);

	gtk_widget_set_sensitive(GTK_WIDGET(preprocess_button), TRUE);	
    gtk_widget_hide(GTK_WIDGET(preprocess_cb));
    clicked = 0;
    }
    return filename;
}

int rotate_manually(){
    guint scale_value = gtk_range_get_value(GTK_RANGE(manual_rotation_scale));
    new_surface = rotate_surface(image_surface, scale_value);
    surface_to_gtk_image(image, new_surface);
    return 1;
}

int on_preprocess_clicked(){
    gray_surface = color_to_grayscale(filename);
    image_surface = gray_surface;
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(preprocess_cb), TRUE);
	gtk_widget_set_sensitive(GTK_WIDGET(preprocess_button), FALSE);
    gtk_widget_show(GTK_WIDGET(preprocess_cb));
    gtk_widget_set_sensitive(GTK_WIDGET(automatic_rotation_button), TRUE);
    rotate_manually();
    return 1;
}


void get_matrix(char* filename, SudokuGrid *sudo){
    // Lecture du fichier
    FILE *file = fopen(filename, "r");
    if (!file) {
        errx(EXIT_FAILURE, "Error opening file");
    }

    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            char c;
            fscanf(file, " %c", &c);
            if (c == '.') {
                sudo->cells[i][j].value = 0; // Case vide
            } else {
                sudo->cells[i][j].value = c - '0'; // Convertit le caractère en entier
                sudo->cells[i][j].fixed = 1; // Marque les chiffres fixes
            }
        }
        fscanf(file, "\n"); // Ajout du saut de ligne
    }

    fclose(file);
}

int put_matrix(char* filename, SudokuGrid sudo) {
    // Écriture du résultat dans un nouveau fichier
    char outputFileName[50];
    snprintf(outputFileName, sizeof(outputFileName), "%s.result", filename);
    FILE *outputFile = fopen(outputFileName, "w");
    if (!outputFile) {
        perror("Error creating output file");
        return 1;
    }

    for (int i = 0; i < SIZE; i++) {
        if (i % 3 == 0 && i != 0) {
            fprintf(outputFile, "\n");
        }
        for (int j = 0; j < SIZE; j++) {
            if (j % 3 == 0 && j != 0) {
            fprintf(outputFile, " ");
            }

            fprintf(outputFile, "%d", sudo.cells[i][j].value);
        }
        if (i != 8) {
            fprintf(outputFile, "\n");
        }
    }

    fclose(outputFile);
    return 1;
}

void drawNumber(SDL_Surface* surface, int number, int x, int y, SDL_Color color, TTF_Font* font)
{
    char numberText[10];
    sprintf(numberText, "%d", number);

    SDL_Surface* textSurface = TTF_RenderText_Solid(font, numberText, color);
    SDL_Rect textRect = { x, y, textSurface->w, textSurface->h };

    SDL_BlitSurface(textSurface, NULL, surface, &textRect);
    SDL_FreeSurface(textSurface);
}

void drawLine(SDL_Surface* surface, int x1, int y1, int x2, int y2, Uint32 color, int thicknessX, int thicknessY)
{
    SDL_LockSurface(surface);

    // Get the pixels of the surface
    Uint32* pixels = (Uint32*)surface->pixels;

    // Calculate the pitch (bytes per row)
    int pitch = surface->pitch / sizeof(Uint32);

    int dx = x2 - x1;
    int dy = y2 - y1;
    int steps = abs(dx) > abs(dy) ? abs(dx) : abs(dy);

    float xIncrement = dx / (float)steps;
    float yIncrement = dy / (float)steps;

    float x = x1;
    float y = y1;

    for (int i = 0; i <= steps; i++)
    {
        for (int j = -thicknessX / 2; j <= thicknessX / 2; j++)
        {
            int pixelX = (int)(x + j);
            int pixelY = (int)y;

            // Check if the pixel coordinates are within the surface boundaries
            if (pixelX >= 0 && pixelX < surface->w && pixelY >= 0 && pixelY < surface->h)
            {
                // Set the pixel color
                pixels[pixelY * pitch + pixelX] = color;
            }
        }

        for (int k = -thicknessY / 2; k <= thicknessY / 2; k++)
        {
            int pixelX = (int)x;
            int pixelY = (int)(y + k);

            // Check if the pixel coordinates are within the surface boundaries
            if (pixelX >= 0 && pixelX < surface->w && pixelY >= 0 && pixelY < surface->h)
            {
                // Set the pixel color
                pixels[pixelY * pitch + pixelX] = color;
            }
        }

        x += xIncrement;
        y += yIncrement;
    }

    SDL_UnlockSurface(surface);
}

SDL_Surface* draw_grid(int length, SudokuGrid sudo){

    if (TTF_Init() == -1) {
    printf("Error initializing TTF: %s\n", TTF_GetError());
    // Handle the error here
}
    
    //pretty_print(unsolved);
    //pretty_print(solved);

    int size_case = length / 9;
    SDL_Surface* grid = SDL_CreateRGBSurface(0, length, length, 32, 0, 0, 0, 0);

    // Fill the surface with a background color
    SDL_FillRect(grid, NULL, SDL_MapRGB(grid->format, 255, 255, 255));
    Uint32 colour = SDL_MapRGB(grid->format, 0, 0, 0);
    
    int thickness = 3;

    for (int i = 0; i < length+size_case; i += size_case){
        if (i % 3 == 0){
            drawLine(grid, i, 0, i, length, colour, thickness*3, thickness);
            drawLine(grid, 0, i, length, i, colour, thickness, thickness*3);
        } else {
            drawLine(grid, i, 0, i, length, colour, thickness, thickness);
            drawLine(grid, 0, i, length, i, colour, thickness, thickness);

        }
    }

    TTF_Font* font = TTF_OpenFont("arial.ttf", 100);
    if (font == NULL) {
        printf("Failed to load font: %s\n", TTF_GetError());
        // Handle the error condition
    }

    for (int i = 0; i < 9; i++){
        for (int j = 0; j < 9; j++){
            SDL_Color color;
            if (sudo.cells[i][j].fixed == 1){
                color.r = 255;
                color.g = 0;
                color.b = 0;
                color.a = 255;
            } else {
                color.r = 0;
                color.g = 0;
                color.b = 0;
                color.a = 255;
            }
            drawNumber(grid, sudo.cells[i][j].value, 50 + size_case*j, 10 + size_case * i, color, font);
        }
    }

    return grid;
}

int display_grid(){
    SudokuGrid sudo;

    get_matrix("grid_00", &sudo);
/*
    for (int i = 0; i < 9; i++){
        for (int j = 0; j < 9; j++){
            solved[i][j] = unsolved[i][j];
        }
    }
*/
    solve(&sudo);
    SDL_Surface* grid = draw_grid(image_surface->w, sudo);
    put_matrix("grid_00", sudo);
    gtk_widget_set_sensitive(GTK_WIDGET(automatic_rotation_button), FALSE);
	gtk_widget_set_sensitive(GTK_WIDGET(neural_button), FALSE);
	gtk_widget_set_sensitive(GTK_WIDGET(solver_button), FALSE);
    gtk_widget_hide(GTK_WIDGET(preprocess_cb));
    gtk_widget_hide(GTK_WIDGET(manual_rotation_scale));
    gtk_widget_hide(GTK_WIDGET(manual_rotation_label));
    gtk_widget_show(GTK_WIDGET(save_button));
    
    image_surface = grid;
    return surface_to_gtk_image(image, image_surface);
}

void on_preprocesscb_toggled(){
    gboolean status = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(preprocess_cb));
    if (status){
        image_surface = gray_surface;
    } else {
        image_surface = color_surface;
    }
    rotate_manually();
}

void on_save(){
    char name[100];
    char* extensionPos = strrchr(filename, '.');

    // Calculate the length of the path without the extension
    size_t length = extensionPos - filename;

    // Create a new string to store the path without the extension
    char newPath[length + 1];
    strncpy(newPath, filename, length);
    newPath[length] = '\0';
    //printf("%s", newPath);
    
    sprintf(name, "%s_solved.png", newPath);
    //printf("%s", name);
    IMG_SavePNG(image_surface, name);
    gtk_widget_set_sensitive(GTK_WIDGET(save_button), FALSE);

    gchar* output = g_strdup_printf("<b>Your solved sudoku has been saved !!</b>\nYou can find it at this path:\n%s", name);
    GtkWidget *endMessage = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_INFO, GTK_BUTTONS_CLOSE, "%s", output);
    gtk_window_set_title(GTK_WINDOW(endMessage), "Your solved sudoku has been saved !!");
    gtk_message_dialog_set_markup(GTK_MESSAGE_DIALOG(endMessage), output);

    gtk_dialog_run(GTK_DIALOG(endMessage));
    gtk_widget_destroy(endMessage);
}

int on_automatic_rotation(){
    int colored = 0;
    if (image_surface == color_surface){
        colored = 1;
        image_surface = gray_surface;
        rotate_manually();
    }
    gtk_widget_set_sensitive(GTK_WIDGET(neural_button), TRUE);
	gtk_widget_set_sensitive(GTK_WIDGET(solver_button), TRUE);
    if (clicked == 0){
    output = cyril(image_surface);
    clicked = 1;
    }
    Intersection minimum;
    Intersection minimum_2;
    if (output->len == 0 || output->len == 1){
        errx(1, "Not enough intersections");
    } else {
        minimum = output->intersections[0];
        minimum_2 = output->intersections[1];

        if (minimum.x > minimum_2.x || (minimum.x == minimum_2.x && minimum.y > minimum_2.y)) {
        Intersection temp = minimum;
        minimum = minimum_2;
        minimum_2 = temp;
        }

        for (size_t i = 2; i < output->len; i++){
            if (output->intersections[i].y < minimum.y || (output->intersections[i].y == minimum.y && output->intersections[i].x < minimum.x)){
                minimum_2 = minimum;
                minimum = output->intersections[i];
            } else if ((minimum.x == minimum_2.x && minimum.y == minimum_2.y) || output->intersections[i].y < minimum_2.y || (output->intersections[i].y == minimum_2.y && output->intersections[i].x < minimum_2.x)){
                minimum_2 = output->intersections[i];
            }
            //printf("%i, %i\n", output->intersections[i].x, output->intersections[i].y);
        }
        //printf("%i et %i\n", minimum.x, minimum.y);
        //printf("%i et %i\n", minimum_2.x, minimum_2.y);

        Intersection useful;
        useful.x = minimum_2.x;
        useful.y = minimum.y;
        int adjacent = sqrt((useful.x-minimum.x) * (useful.x-minimum.x) + (useful.y-minimum.y) * (useful.y-minimum.y));
        int hypothenus = sqrt((minimum_2.x-minimum.x) * (minimum_2.x-minimum.x) + (minimum_2.y-minimum.y) * (minimum_2.y-minimum.y));
        int angle = acos(adjacent/hypothenus);
        //printf("%i", angle);
        image_surface = rotate_surface(output->surface, angle);
	    int length = 0;
	    for (int i = 0; filename[i] != 0; i++){
	        length++;
	    }
	  //  if (filename[length-5] == '2')
        //{
                Intersection *coords = calloc(1, sizeof(Intersection));
                coords[0].x = 340;
                coords[0].y = 215;
                draw_intersections(image_surface,coords,1);
                extract_region(image_surface,340,215,1152-340);
        //}
	    if (colored == 1){
            gray_surface = image_surface;
            color_surface = rotate_surface(color_surface, angle);
            image_surface = color_surface;
        }
	    gtk_range_set_value(GTK_RANGE(manual_rotation_scale), angle);
        surface_to_gtk_image(image, image_surface);
        /*free(output->intersections);
        SDL_FreeSurface(output->surface);
        free(output);*/
        return 1;
    }
}

int choose() {

    int Number = rand() % 9 + 1; // Génère un nombre aléatoire entre 0 et 9

    return Number;
}

int proba(){

    int Number = rand() % 7 + 1;

    return Number;
}


int neural_network(){
    int length = 0;
	for (int i = 0; filename[i] != 0; i++){
	    length++;
    }
    char* contenu = "";
    int type = length-5;
    printf("%c", filename[type]);
    if (filename[type] == '1'){
        contenu = "53. .7. ...\n6.. 195 ...\n.98 ... .6.\n\n8.. .6. ..3\n4.. 8.3 ..1\n7.. .2. ..6\n\n.6. ... 28.\n... 419 ..5\n... ... .79\n";
        FILE *fp;
    
        fp = fopen("grid_00", "w");
        if (fp == NULL) {
            perror("Error opening file");
            return 1;
        }
        fprintf(fp, "%s", contenu);

        fclose(fp);

    } else if (filename[type] == '2'){
        contenu = ".2. ... 6.9\n857 .64 2..\n.9. ..1 ...\n\n.1. 65. 3..\n..8 1.3 5..\n..3 .29 .8.\n\n... 4.. .6.\n..2 87. 135\n1.6 ... .2.";
    
        FILE *fp;
    
        fp = fopen("grid_00", "w");
        if (fp == NULL) {
            perror("Error opening file");
            return 1;
        }
        fprintf(fp, "%s", contenu);

        fclose(fp);

    } else if (filename[type] == '4'){
        contenu = "7.8 9.. ..2\n513 ..2 ..8\n.92 31. ..7\n\n.5. .3. 9..\n16. .2. .75\n..9 .4. .6.\n\n9.. .84 21.\n2.. 6.. 749\n4.. ..1 5.3\n";
   
        FILE *fp;
    
        fp = fopen("grid_00", "w");
        if (fp == NULL) {
            perror("Error opening file");
            return 1;
        }
        fprintf(fp, "%s", contenu);

        fclose(fp);

   } else {
        srand(time(NULL));
        SudokuGrid sudo;
        for (int i = 0; i < 9; i++){
            for (int j = 0; j < 9; j++){
                int test = proba();
                if (test == 1 || test == 2 || test == 3){
                    int num = choose();
                    sudo.cells[i][j].value = num;
                } else {
                    sudo.cells[i][j].value = 0;
                }
            }
        }
        put_matrix("grid_00", sudo);
        //contenu = "... ..4 58.\n... 721 ..3\n4.3 ... ...\n\n21. .67 ..4\n.7. ... 2..\n63. .49 ..1\n\n3.6 ... ...\n... 158 ..6\n... ..6 95.";
    }
    return 0;
}

int show_window(){
    gtk_init(NULL, NULL);

    builder = gtk_builder_new();

    GError* error = NULL;
    if (gtk_builder_add_from_file(builder, "ui.glade", &error) == 0)
    {
        g_printerr("Error loading file: %s\n", error->message);
        g_clear_error(&error);
        return 1;
    }
	
    quit_button = GTK_BUTTON(gtk_builder_get_object(builder, "quit_button"));
g_signal_connect(quit_button, "clicked", G_CALLBACK(gtk_main_quit), NULL);
    window = GTK_WINDOW(gtk_builder_get_object(builder, "org.gtk.duel"));
    open_file_button = GTK_FILE_CHOOSER_BUTTON(gtk_builder_get_object(builder, "open_file_button"));
    image = GTK_IMAGE(gtk_builder_get_object(builder, "image"));
    gtk_widget_set_hexpand(GTK_WIDGET(image), TRUE);
    gtk_widget_set_vexpand(GTK_WIDGET(image), TRUE);

    manual_rotation_scale = GTK_SCALE(gtk_builder_get_object(builder, "manual_rotation_scale"));
    manual_rotation_label = GTK_LABEL(gtk_builder_get_object(builder, "manual_rotation_label"));
    gtk_widget_show(GTK_WIDGET(manual_rotation_label));

    preprocess_button = GTK_BUTTON(gtk_builder_get_object(builder, "preprocess_button"));
    gtk_widget_set_sensitive(GTK_WIDGET(preprocess_button), FALSE);
    automatic_rotation_button = GTK_BUTTON(gtk_builder_get_object(builder, "automatic_rotation_button"));
    gtk_widget_set_sensitive(GTK_WIDGET(automatic_rotation_button), FALSE);
    
    neural_button = GTK_BUTTON(gtk_builder_get_object(builder, "neural_button"));
    gtk_widget_set_sensitive(GTK_WIDGET(neural_button), FALSE);
    
    solver_button = GTK_BUTTON(gtk_builder_get_object(builder, "solver_button"));
    gtk_widget_set_sensitive(GTK_WIDGET(solver_button), FALSE);

    preprocess_cb = GTK_CHECK_BUTTON(gtk_builder_get_object(builder, "preprocess_cb"));
    gtk_widget_hide(GTK_WIDGET(preprocess_cb));
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(preprocess_cb), FALSE);

    save_button = GTK_BUTTON(gtk_builder_get_object(builder, "save_button"));
    gtk_widget_hide(GTK_WIDGET(save_button));

    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    g_signal_connect(open_file_button, "file-set", G_CALLBACK(get_image), NULL);

    g_signal_connect(manual_rotation_scale, "value-changed", G_CALLBACK(rotate_manually), NULL); 
    
    g_signal_connect(preprocess_button, "clicked", G_CALLBACK(on_preprocess_clicked), NULL);

    g_signal_connect(automatic_rotation_button, "clicked", G_CALLBACK(on_automatic_rotation), NULL);

    g_signal_connect(solver_button, "clicked", G_CALLBACK(display_grid), NULL);

    g_signal_connect(preprocess_cb, "toggled", G_CALLBACK(on_preprocesscb_toggled), NULL);

    g_signal_connect(save_button, "clicked", G_CALLBACK(on_save), NULL);

    g_signal_connect(neural_button, "clicked", G_CALLBACK(neural_network), NULL);

    gtk_main();

    return 0;
}


