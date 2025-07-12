import pygame
import time

# Init pygame
pygame.init()
pygame.joystick.init()

if pygame.joystick.get_count() == 0:
    print("No PS4 controller detected.")
    exit()

joystick = pygame.joystick.Joystick(0)
joystick.init()
print(f"Controller: {joystick.get_name()}")

def apply_deadzone(val, threshold=0.1):
    return 0.0 if abs(val) < threshold else val

try:
    while True:
        pygame.event.pump()
        throttle = -apply_deadzone(joystick.get_axis(1))  # Left stick Y
        steering_2 = apply_deadzone(joystick.get_axis(2))
        steering_3 = apply_deadzone(joystick.get_axis(3))
        steering = steering_3 if abs(steering_3) > 0 else steering_2

        print(f"Throttle: {throttle:.2f} | Steering: {steering:.2f}")
        time.sleep(0.1)

except KeyboardInterrupt:
    pygame.quit()
    print("Stopped.") 