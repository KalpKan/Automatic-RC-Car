import os
import shutil
import random
import logging
from typing import List

# data_prep_split.py
# Shuffles and splits the green and non_green datasets into train and validation sets (80/20 split).
# Prepares balanced data splits for robust machine learning training and evaluation.

def split_folder(src_folder: str, train_folder: str, val_folder: str, val_ratio: float = 0.2) -> None:
    files = [f for f in os.listdir(src_folder) if f.lower().endswith('.jpg')]
    random.shuffle(files)
    n_val = int(len(files) * val_ratio)
    val_files = files[:n_val]
    train_files = files[n_val:]
    for split, split_files, out_folder in [('train', train_files, train_folder), ('val', val_files, val_folder)]:
        os.makedirs(out_folder, exist_ok=True)
        for fname in split_files:
            src_img = os.path.join(src_folder, fname)
            dst_img = os.path.join(out_folder, fname)
            shutil.move(src_img, dst_img)
            # Move annotation if exists
            ann_src = os.path.splitext(src_img)[0] + '.txt'
            ann_dst = os.path.splitext(dst_img)[0] + '.txt'
            if os.path.exists(ann_src):
                shutil.move(ann_src, ann_dst)
        logging.info(f"{split_folder.__name__}: {split} {out_folder}: {len(split_files)} images")

def split_dataset():
    random.seed(42)
    split_folder('data/green', 'data/train/green', 'data/val/green')
    split_folder('data/non_green', 'data/train/non_green', 'data/val/non_green')

if __name__ == "__main__":
    logging.basicConfig(level=logging.INFO, format='%(levelname)s: %(message)s')
    split_dataset() 