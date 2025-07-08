# data_prep_annotate.py
# Generates YOLO-style bounding box annotations for the largest green region in each green image.
# Provides localization data for object detection training.
import os
import cv2
import numpy as np
import logging
from data_prep_green_threshold import LOWER_GREEN, UPPER_GREEN

def annotate_green_bounding_box(image_path: str) -> None:
    img = cv2.imread(image_path)
    if img is None:
        logging.error(f"Failed to read image: {image_path}")
        return
    hsv = cv2.cvtColor(img, cv2.COLOR_BGR2HSV)
    mask = cv2.inRange(hsv, LOWER_GREEN, UPPER_GREEN)
    contours, _ = cv2.findContours(mask, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
    if not contours:
        logging.info(f"No green region found in {image_path}")
        return
    largest = max(contours, key=cv2.contourArea)
    x, y, w, h = cv2.boundingRect(largest)
    # Save annotation in YOLO-style: x y w h (pixels)
    txt_path = os.path.splitext(image_path)[0] + '.txt'
    with open(txt_path, 'w') as f:
        f.write(f"{x} {y} {w} {h}\n")
    logging.debug(f"Annotated {image_path}: {x} {y} {w} {h}")

if __name__ == "__main__":
    logging.basicConfig(level=logging.DEBUG, format='%(levelname)s: %(message)s')
    green_dir = 'data/green'
    for fname in os.listdir(green_dir):
        if not fname.lower().endswith('.jpg'):
            continue
        fpath = os.path.join(green_dir, fname)
        annotate_green_bounding_box(fpath) 