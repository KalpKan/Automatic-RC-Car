import os
import cv2
import logging
from typing import List

# data_prep_clean.py
# Resizes and standardizes all images in the green and non_green folders to 320x240 JPGs.
# Ensures consistent input for machine learning training.

def clean_and_resize_images(folder: str, size: tuple = (320, 240)) -> None:
    for fname in os.listdir(folder):
        fpath = os.path.join(folder, fname)
        if not os.path.isfile(fpath):
            continue
        try:
            img = cv2.imread(fpath)
            if img is None:
                logging.error(f"Failed to read image: {fpath}")
                continue
            resized = cv2.resize(img, size)
            # Save as .jpg, overwrite original (change extension if needed)
            new_fpath = os.path.splitext(fpath)[0] + '.jpg'
            cv2.imwrite(new_fpath, resized, [int(cv2.IMWRITE_JPEG_QUALITY), 95])
            if new_fpath != fpath and os.path.exists(fpath):
                os.remove(fpath)
            logging.debug(f"Processed and saved: {new_fpath}")
        except Exception as e:
            logging.error(f"Error processing {fpath}: {e}")

if __name__ == "__main__":
    logging.basicConfig(level=logging.DEBUG, format='%(levelname)s: %(message)s')
    for folder in ['data/green', 'data/non_green']:
        clean_and_resize_images(folder) 