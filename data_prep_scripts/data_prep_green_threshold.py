import cv2
import numpy as np
import logging
from typing import Any

# HSV bounds for green
LOWER_GREEN = np.array([50, 100, 100])
UPPER_GREEN = np.array([70, 255, 255])

# data_prep_green_threshold.py
# Defines HSV green color thresholds and provides a helper function to classify images as green or non-green based on green pixel ratio.
# This is a core step in filtering images for green object detection.

def is_green_image(path: str, threshold: float = 0.05) -> bool:
    """Return True if image at path contains >= threshold fraction of green pixels."""
    try:
        img = cv2.imread(path)
        if img is None:
            logging.error(f"Failed to read image: {path}")
            return False
        hsv = cv2.cvtColor(img, cv2.COLOR_BGR2HSV)
        mask = cv2.inRange(hsv, LOWER_GREEN, UPPER_GREEN)
        green_pixels = np.count_nonzero(mask)
        total_pixels = mask.size
        green_ratio = green_pixels / total_pixels
        result = green_ratio >= threshold
        logging.debug(f"{path}: green_ratio={green_ratio:.2%}, result={'GREEN' if result else 'NON_GREEN'}")
        return result
    except Exception as e:
        logging.error(f"Error processing {path}: {e}")
        return False

if __name__ == "__main__":
    logging.basicConfig(level=logging.DEBUG, format='%(levelname)s: %(message)s')
    # Example usage: test on all files in data/raw_images
    import os
    raw_dir = 'data/raw_images'
    for fname in os.listdir(raw_dir):
        fpath = os.path.join(raw_dir, fname)
        if not os.path.isfile(fpath):
            continue
        is_green_image(fpath) 