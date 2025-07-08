# green_object_realtime_demo.py
# Real-time green object detection demo using webcam. Displays bounding boxes, mask coverage, and allows HSV threshold tuning via sliders. Logs mask coverage per frame.

import cv2
import numpy as np
import logging

logging.basicConfig(level=logging.INFO, format='%(levelname)s: %(message)s')

# ENGINEERING: Broad HSV bounds for green (covers yellow-green to blue-green)
# These can be further tuned or learned from data
DEFAULT_LOWER_GREEN = np.array([35, 40, 40])   # H, S, V
DEFAULT_UPPER_GREEN = np.array([85, 255, 255])

WINDOW = 'Green Object Detection'


def nothing(x: int) -> None:
    pass

def create_hsv_sliders() -> None:
    cv2.namedWindow(WINDOW)
    cv2.createTrackbar('H_low', WINDOW, DEFAULT_LOWER_GREEN[0], 179, nothing)
    cv2.createTrackbar('S_low', WINDOW, DEFAULT_LOWER_GREEN[1], 255, nothing)
    cv2.createTrackbar('V_low', WINDOW, DEFAULT_LOWER_GREEN[2], 255, nothing)
    cv2.createTrackbar('H_high', WINDOW, DEFAULT_UPPER_GREEN[0], 179, nothing)
    cv2.createTrackbar('S_high', WINDOW, DEFAULT_UPPER_GREEN[1], 255, nothing)
    cv2.createTrackbar('V_high', WINDOW, DEFAULT_UPPER_GREEN[2], 255, nothing)

def get_hsv_bounds() -> tuple:
    h_low = cv2.getTrackbarPos('H_low', WINDOW)
    s_low = cv2.getTrackbarPos('S_low', WINDOW)
    v_low = cv2.getTrackbarPos('V_low', WINDOW)
    h_high = cv2.getTrackbarPos('H_high', WINDOW)
    s_high = cv2.getTrackbarPos('S_high', WINDOW)
    v_high = cv2.getTrackbarPos('V_high', WINDOW)
    return (np.array([h_low, s_low, v_low]), np.array([h_high, s_high, v_high]))

def main() -> None:
    cap = cv2.VideoCapture(1)
    if not cap.isOpened():
        logging.error('Could not open webcam.')
        return
    create_hsv_sliders()
    while True:
        ret, frame = cap.read()
        if not ret:
            logging.error('Failed to grab frame.')
            break
        hsv = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)
        lower, upper = get_hsv_bounds()
        # ENGINEERING: Gaussian blur for smoother mask
        blurred = cv2.GaussianBlur(hsv, (7, 7), 0)
        mask = cv2.inRange(blurred, lower, upper)
        # ENGINEERING: Morphological opening (remove small noise)
        kernel = np.ones((5, 5), np.uint8)
        mask = cv2.morphologyEx(mask, cv2.MORPH_OPEN, kernel, iterations=2)
        mask = cv2.dilate(mask, kernel, iterations=1)
        # Find contours
        contours, _ = cv2.findContours(mask, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
        largest_coverage = 0
        if contours:
            largest = max(contours, key=cv2.contourArea)
            x, y, w, h = cv2.boundingRect(largest)
            cv2.rectangle(frame, (x, y), (x + w, y + h), (0, 255, 0), 2)
            largest_coverage = 100 * cv2.contourArea(largest) / mask.size
        # Mask coverage
        green_pixels = np.count_nonzero(mask)
        total_pixels = mask.size
        coverage = 100 * green_pixels / total_pixels
        cv2.putText(frame, f'Total green: {coverage:.1f}%', (10, 30), cv2.FONT_HERSHEY_SIMPLEX, 0.8, (0, 255, 0), 2)
        cv2.putText(frame, f'Largest object: {largest_coverage:.1f}%', (10, 60), cv2.FONT_HERSHEY_SIMPLEX, 0.8, (0, 200, 0), 2)
        logging.info(f'Total green: {coverage:.2f}%, Largest object: {largest_coverage:.2f}%')
        cv2.imshow(WINDOW, frame)
        cv2.imshow('Mask', mask)
        key = cv2.waitKey(1) & 0xFF
        if key == 27:  # ESC to quit
            break
    cap.release()
    cv2.destroyAllWindows()

# ML/DATA-DRIVEN IMPROVEMENT:
# - Analyze your dataset to find the HSV distribution of green pixels.
# - Use k-means or histogram analysis to set optimal bounds.
# - Optionally, train a simple classifier (e.g., SVM, kNN) on pixel HSV values for robust green detection.

if __name__ == '__main__':
    main() 