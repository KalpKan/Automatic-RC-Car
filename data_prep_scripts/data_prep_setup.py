import os
import logging
from typing import List

# Folder names
FOLDERS = [
    'data/raw_images',
    'data/green',
    'data/non_green',
]

# data_prep_setup.py
# Sets up the folder structure and initializes logging for the data filtering pipeline.
# Ensures all required directories exist before processing images for green object detection.

def ensure_folders_exist(folders: List[str]) -> None:
    for folder in folders:
        abs_path = os.path.abspath(folder)
        os.makedirs(abs_path, exist_ok=True)
        logging.debug(f"Ensured folder exists: {abs_path}")

def log_file_counts(folders: List[str]) -> None:
    for folder in folders:
        abs_path = os.path.abspath(folder)
        count = len([f for f in os.listdir(abs_path) if os.path.isfile(os.path.join(abs_path, f))])
        print(f"{abs_path}: {count} files")

if __name__ == "__main__":
    # Configure logger to output to console
    logging.basicConfig(level=logging.DEBUG, format='%(levelname)s: %(message)s')
    print("--- Data Preparation Setup ---")
    ensure_folders_exist(FOLDERS)
    log_file_counts(FOLDERS) 