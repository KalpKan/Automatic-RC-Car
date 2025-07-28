import os
import shutil
import logging
from data_prep_green_threshold import is_green_image
from typing import List
import sys

IMAGE_EXTS = {'.jpg', '.jpeg', '.png', '.bmp', '.tiff'}

def find_all_images(root: str) -> List[str]:
    image_files = []
    for dirpath, _, filenames in os.walk(root):
        for fname in filenames:
            if os.path.splitext(fname)[1].lower() in IMAGE_EXTS:
                image_files.append(os.path.join(dirpath, fname))
    return image_files

def batch_classify_images_recursive(raw_root: str, green_dir: str, non_green_dir: str) -> None:
    image_files = find_all_images(raw_root)
    logging.info(f"Found {len(image_files)} images in {raw_root}")
    for fpath in image_files:
        try:
            if is_green_image(fpath):
                shutil.copy(fpath, green_dir)
                logging.debug(f"{fpath}: GREEN (copied to {green_dir})")
            else:
                shutil.copy(fpath, non_green_dir)
                logging.debug(f"{fpath}: NON_GREEN (copied to {non_green_dir})")
        except Exception as e:
            logging.error(f"Error processing {fpath}: {e}")

if __name__ == "__main__":
    logging.basicConfig(level=logging.DEBUG, format='%(levelname)s: %(message)s')
    # Allow user to specify the root directory to process
    if len(sys.argv) > 1:
        raw_root = sys.argv[1]
    else:
        raw_root = 'data/raw_images/Fruit_Flower_Veg'
    green_dir = 'data/green'
    non_green_dir = 'data/non_green'
    batch_classify_images_recursive(raw_root, green_dir, non_green_dir) 