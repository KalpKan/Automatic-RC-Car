# camera_test.py
# Minimal script to test OpenCV camera access and frame grabbing on all common indices.

import cv2

for idx in range(3):
    print(f"Testing camera index {idx}...")
    cap = cv2.VideoCapture(idx)
    if not cap.isOpened():
        print(f"  Camera {idx} not opened.")
        continue
    ret, frame = cap.read()
    print(f"  Frame grabbed: {ret}")
    cap.release() 