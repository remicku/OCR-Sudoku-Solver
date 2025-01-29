import numpy as np
import cv2 as cv
from PIL import Image, ImageFont, ImageDraw
from random import randint
import requests, os

font_size = 28 # px
KEY = "AIzaSyA4zS6S5OKrWWTOV-9XsYKGTlj0F0hVvcg"

def treatment(path):
    img = cv.imread(path, cv.IMREAD_GRAYSCALE)

    img = cv.Canny(img, 100, 200)
    img = cv.resize(img, (28, 28))


def dlDigits(path):
    font = ImageFont.truetype(path, font_size)
    for i in range(10):
        try:
            img = Image.new('RGBA', (font_size, font_size), color = (0, 0, 0))
            d = ImageDraw.Draw(img)
            d.text((0, 0), str(i), fill=(255, 255, 255), font=font)
            img.save(f"./data/{i}/{randint(0, 10000000)}.png")
        except:
            pass

def dlFonts(url):
    r  = requests.get(url)
    open(f"ttf/{randint(0, 10000000)}.ttf", 'wb').write(r.content)

def discoverFont():
    r = requests.get(f"https://www.googleapis.com/webfonts/v1/webfonts?key={KEY}")
    fonts = r.json()["items"]
    for font in fonts:
        for f in font["files"]:
                dlFonts(font["files"][f])


#discoverFont()
for f in os.listdir("ttf"):
    dlDigits(f"ttf/{f}")
