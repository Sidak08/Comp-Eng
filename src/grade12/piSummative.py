# MicroPython (Raspberry Pi Pico) version of your Arduino sketch
# - HC-SR04 ultrasonic (rain detector)
# - Servo wiper
# - Button (single tap / double tap) using IRQ + ring buffer
# - LDR (photoresistor) -> headlight LED ON when LDR < 600
# - ILI9341 TFT status screen (optional; falls back to prints if driver missing)

import time

import micropython
from machine import ADC, PWM, SPI, Pin

# -----------------------------------------
# Optional TFT driver import (ILI9341)
# -----------------------------------------
# Different ILI9341 MicroPython libraries exist.
# Many expose methods like: fill(), fill_rect(), text(), hline(), etc.
# If import fails, we use a stub that just prints updates.
try:
    from ili9341 import ILI9341, color565  # common driver style

    TFT_AVAILABLE = True
except Exception:
    TFT_AVAILABLE = False

    def color565(r, g, b):
        # Fallback (not actually used without a real driver)
        return 0

    class ILI9341:
        def __init__(self, *args, **kwargs):
            pass

        def fill(self, c):
            pass

        def fill_rect(self, x, y, w, h, c):
            pass

        def text(self, s, x, y, c):
            pass

        def hline(self, x, y, w, c):
            pass


# -----------------------------------------
# Logging helpers (turn off if too noisy)
# -----------------------------------------
LOG_DEBUG = True
LOG_INFO = True
LOG_WARN = True


def dlog(msg):
    if LOG_DEBUG:
        print("[DEBUG]", msg)


def ilog(msg):
    if LOG_INFO:
        print("[INFO]", msg)


def wlog(msg):
    if LOG_WARN:
        print("[WARN]", msg)


# -----------------------------------------
# Pin mapping (same as your setup)
# -----------------------------------------
SERVO_PIN = 18
BUTTON_PIN = 17

TRIG_PIN = 27
ECHO_PIN = 26

# RGB LED pins (you requested these)
LED_R_PIN = 2
LED_G_PIN = 3
LED_B_PIN = 4

PHOTO_ADC_CH = 2  # ADC2 on Pico = GP28
HEADLIGHT_LED_PIN = 6  # GP6

# TFT pins (hardware SPI in MicroPython is fast and works well)
TFT_SCK = 10
TFT_MOSI = 11
TFT_MISO = 12  # if your TFT has SDO/MISO, wire it; if not, you can leave it unconnected
TFT_CS = 13
TFT_DC = 14
TFT_RST = 15


# -----------------------------------------
# Behavior constants
# -----------------------------------------
# LDR threshold (your request)
LDR_THRESHOLD = 600  # headlight ON when LDR < 600

# Servo movement
LEFT_ANGLE = 20
RIGHT_ANGLE = 180
SERVO_MOVE_DELAY_MS = 150

# Continuous wiper timing
CONTINUOUS_SWEEP_INTERVAL_MS = 700

# Double tap window
DOUBLE_TAP_WINDOW_US = 500_000  # 500ms

# Ultrasonic timing
ULTRASONIC_TIMEOUT_US = 30_000

# Rain anti-flicker settings (important!)
RAIN_ON_CM = 15.0  # turn rain ON below this
RAIN_OFF_CM = 18.0  # turn rain OFF above this (hysteresis)
RAIN_HOLD_MS = 1200  # keep rain ON through brief timeouts / noise

# Ultrasonic filtering (median of 3)
ULTRASONIC_SAMPLES = 3
ULTRASONIC_SAMPLE_GAP_MS = 6

# Skip ultrasonic reads right after servo sweep (servo noise window)
ULTRASONIC_COOLDOWN_AFTER_SWEEP_MS = 180


# -----------------------------------------
# Hardware setup
# -----------------------------------------
# RGB LED (common cathode: HIGH turns color ON)
led_r = Pin(LED_R_PIN, Pin.OUT)
led_g = Pin(LED_G_PIN, Pin.OUT)
led_b = Pin(LED_B_PIN, Pin.OUT)


def set_rgb(r, g, b):
    # r/g/b are booleans or 0/1
    led_r.value(1 if r else 0)
    led_g.value(1 if g else 0)
    led_b.value(1 if b else 0)


# Start idle = green
set_rgb(0, 1, 0)

# Headlight LED
headlight_led = Pin(HEADLIGHT_LED_PIN, Pin.OUT)
headlight_led.value(0)

# LDR ADC
ldr_adc = ADC(PHOTO_ADC_CH)

# Button (pull-down, rising edge IRQ)
button = Pin(BUTTON_PIN, Pin.IN, Pin.PULL_DOWN)

# Ultrasonic pins
trig = Pin(TRIG_PIN, Pin.OUT)
echo = Pin(ECHO_PIN, Pin.IN)
trig.value(0)

# Servo PWM @ 50Hz
servo_pwm = PWM(Pin(SERVO_PIN))
servo_pwm.freq(50)


def angle_to_duty_u16(angle):
    """
    Convert 0..180 degrees to a 50Hz servo pulse duty.
    Typical servos: ~500us (0 deg) to ~2500us (180 deg)
    Period at 50Hz = 20,000us
    """
    angle = max(0, min(180, angle))
    min_us = 500
    max_us = 2500
    pulse_us = min_us + (max_us - min_us) * angle // 180
    duty = int(pulse_us * 65535 // 20000)
    return duty


def set_servo_angle(angle):
    duty = angle_to_duty_u16(angle)
    dlog(f"Servo -> {angle} deg (duty_u16={duty})")
    servo_pwm.duty_u16(duty)
    time.sleep_ms(SERVO_MOVE_DELAY_MS)


# -----------------------------------------
# TFT setup (optional)
# -----------------------------------------
# Coordinates/layout (landscape-ish)
X_LABEL = 10
X_VALUE = 140
Y_RAIN = 50
Y_MODE = 90
Y_HEAD = 130
Y_LDR = 170

# Color constants (if driver present)
C_BLACK = color565(0, 0, 0)
C_WHITE = color565(255, 255, 255)
C_GREY = color565(80, 80, 80)
C_CYAN = color565(0, 255, 255)
C_YELL = color565(255, 255, 0)
C_GREEN = color565(0, 255, 0)
C_RED = color565(255, 0, 0)

tft = None


def tft_init():
    """
    Initialize TFT. If no driver installed, we just log.
    """
    global tft
    if not TFT_AVAILABLE:
        ilog("TFT driver not found; using console output only.")
        return

    # SPI(1) on Pico: we explicitly set pins
    spi = SPI(
        1,
        baudrate=40_000_000,
        polarity=0,
        phase=0,
        sck=Pin(TFT_SCK),
        mosi=Pin(TFT_MOSI),
        miso=Pin(TFT_MISO),
    )

    cs = Pin(TFT_CS, Pin.OUT)
    dc = Pin(TFT_DC, Pin.OUT)
    rst = Pin(TFT_RST, Pin.OUT)

    # Many ILI9341 drivers take width/height; adjust if your driver differs
    # Common: ILI9341(spi, cs, dc, rst, w=320, h=240, rotation=1)
    try:
        tft = ILI9341(spi, cs=cs, dc=dc, rst=rst, w=320, h=240, rotation=1)
    except TypeError:
        # Some drivers use positional args
        tft = ILI9341(spi, cs, dc, rst)

    tft_draw_static()


def tft_draw_static():
    """
    Draw labels once.
    """
    if not TFT_AVAILABLE or tft is None:
        return

    # Some drivers use fill(), some use fill_rect; we use fill if possible
    try:
        tft.fill(C_BLACK)
    except Exception:
        tft.fill_rect(0, 0, 320, 240, C_BLACK)

    tft.text("WIPER SYSTEM", 10, 10, C_WHITE)
    tft.hline(10, 35, 220, C_GREY)

    tft.text("Raining:", X_LABEL, Y_RAIN, C_WHITE)
    tft.text("Mode:", X_LABEL, Y_MODE, C_WHITE)
    tft.text("Headlight:", X_LABEL, Y_HEAD, C_WHITE)
    tft.text("LDR:", X_LABEL, Y_LDR, C_WHITE)


def tft_write_value(x, y, w, h, text, color):
    """
    Clear a rectangle, then draw new text.
    """
    if not TFT_AVAILABLE or tft is None:
        return
    tft.fill_rect(x, y, w, h, C_BLACK)
    tft.text(text, x, y, color)


# Track last displayed values to avoid constant redraw
tft_last_rain = None
tft_last_mode = None
tft_last_head = None
tft_last_ldr = None


def tft_update(raining_now, mode_str, head_on, ldr_value):
    """
    Update only changed fields on the TFT.
    If driver missing, print changes to console.
    """
    global tft_last_rain, tft_last_mode, tft_last_head, tft_last_ldr

    if not TFT_AVAILABLE or tft is None:
        # Console fallback: print on change only
        changed = False
        if raining_now != tft_last_rain:
            ilog(f"TFT: Raining = {'YES' if raining_now else 'NO'}")
            tft_last_rain = raining_now
            changed = True
        if mode_str != tft_last_mode:
            ilog(f"TFT: Mode = {mode_str}")
            tft_last_mode = mode_str
            changed = True
        if head_on != tft_last_head:
            ilog(f"TFT: Headlight = {'ON' if head_on else 'OFF'}")
            tft_last_head = head_on
            changed = True
        if (tft_last_ldr is None) or abs(ldr_value - tft_last_ldr) >= 10:
            ilog(f"TFT: LDR = {ldr_value}")
            tft_last_ldr = ldr_value
            changed = True
        return

    # Real TFT updates (only on change)
    if raining_now != tft_last_rain:
        tft_last_rain = raining_now
        tft_write_value(
            X_VALUE,
            Y_RAIN,
            170,
            16,
            "YES" if raining_now else "NO",
            C_CYAN if raining_now else C_YELL,
        )
        dlog("TFT: Raining updated")

    if mode_str != tft_last_mode:
        tft_last_mode = mode_str
        tft_write_value(X_VALUE, Y_MODE, 170, 16, mode_str, C_GREEN)
        dlog("TFT: Mode updated")

    if head_on != tft_last_head:
        tft_last_head = head_on
        tft_write_value(
            X_VALUE,
            Y_HEAD,
            170,
            16,
            "ON" if head_on else "OFF",
            C_GREEN if head_on else C_RED,
        )
        dlog("TFT: Headlight updated")

    if (tft_last_ldr is None) or abs(ldr_value - tft_last_ldr) >= 10:
        tft_last_ldr = ldr_value
        tft_write_value(X_VALUE, Y_LDR, 170, 16, str(ldr_value), C_WHITE)
        dlog("TFT: LDR updated")


# -----------------------------------------
# Modes + display mode handling
# -----------------------------------------
MODE_IDLE = 0
MODE_MANUAL_SINGLE = 1
MODE_MANUAL_CONTINUOUS = 2
MODE_AUTO_CONTINUOUS = 3
MODE_AUTO_PLUS_MANUAL = 4


def mode_to_string(m):
    if m == MODE_IDLE:
        return "IDLE"
    if m == MODE_MANUAL_SINGLE:
        return "MANUAL (SINGLE)"
    if m == MODE_MANUAL_CONTINUOUS:
        return "MANUAL (CONT)"
    if m == MODE_AUTO_CONTINUOUS:
        return "AUTO (RAIN)"
    if m == MODE_AUTO_PLUS_MANUAL:
        return "AUTO+MANUAL"
    return "UNKNOWN"


display_mode = MODE_IDLE
display_hold_until_ms = 0  # show MANUAL(SINGLE) for a bit so you can see it


def set_display_mode(m, hold_ms=0):
    global display_mode, display_hold_until_ms
    display_mode = m
    display_hold_until_ms = time.ticks_add(time.ticks_ms(), hold_ms) if hold_ms else 0


# -----------------------------------------
# Button IRQ ring buffer (double tap reliability)
# -----------------------------------------
# NOTE: In MicroPython, avoid allocations in ISR.
EVT_BUF_SZ = 8
evt_buf = [0] * EVT_BUF_SZ
evt_w = 0
evt_r = 0

# Small ISR debounce to ignore bounce spikes
ISR_DEBOUNCE_US = 3000
last_isr_us = 0


def button_isr(pin):
    global evt_w, last_isr_us
    us = time.ticks_us()
    if time.ticks_diff(us, last_isr_us) < ISR_DEBOUNCE_US:
        return
    last_isr_us = us

    nxt = (evt_w + 1) & (EVT_BUF_SZ - 1)
    if nxt == evt_r:
        # Buffer full, drop
        return
    evt_buf[evt_w] = us
    evt_w = nxt


# Attach IRQ
button.irq(trigger=Pin.IRQ_RISING, handler=button_isr)


def pop_button_event_us():
    """
    Pop one event timestamp (us) from the ring buffer.
    """
    global evt_r
    if evt_r == evt_w:
        return None
    us = evt_buf[evt_r]
    evt_r = (evt_r + 1) & (EVT_BUF_SZ - 1)
    return us


# Double tap state
waiting_second = False
first_tap_us = 0
manual_continuous_enabled = False


# -----------------------------------------
# Ultrasonic measurement (median filter)
# -----------------------------------------
last_sweep_ms = 0
last_valid_distance_cm = -1.0
last_rain_seen_ms = 0
was_raining = False


def measure_distance_once_cm():
    """
    Take one HC-SR04 measurement.
    Returns distance in cm, or -1 on timeout.
    """
    trig.value(0)
    time.sleep_us(2)
    trig.value(1)
    time.sleep_us(10)
    trig.value(0)

    # machine.time_pulse_us exists on many MicroPython builds; on some it raises OSError on timeout
    try:
        import machine

        dur = machine.time_pulse_us(echo, 1, ULTRASONIC_TIMEOUT_US)
        if dur < 0:
            return -1.0
    except Exception:
        return -1.0

    # speed of sound ~ 0.0343 cm/us, divide by 2 for round trip
    return (dur / 2.0) * 0.0343


def median_of_three(a, b, c):
    """
    Median of 3 numbers without creating lists (fast, no allocations).
    """
    if a > b:
        a, b = b, a
    if b > c:
        b, c = c, b
    if a > b:
        a, b = b, a
    return b


def measure_distance_filtered_cm():
    """
    Take 3 measurements, reject invalid, return median/average.
    """
    vals = []
    for _ in range(ULTRASONIC_SAMPLES):
        d = measure_distance_once_cm()

        # Reject too-close weirdness and out-of-range noise
        if 2.0 <= d <= 400.0:
            vals.append(d)

        time.sleep_ms(ULTRASONIC_SAMPLE_GAP_MS)

    if not vals:
        return -1.0
    if len(vals) == 1:
        return vals[0]
    if len(vals) == 2:
        return (vals[0] + vals[1]) / 2.0
    return median_of_three(vals[0], vals[1], vals[2])


def compute_raining(got_valid, dist_cm, now_ms):
    """
    Prevent flicker:
    - use hysteresis (RAIN_ON_CM / RAIN_OFF_CM)
    - hold rain ON for RAIN_HOLD_MS through brief timeouts/noise
    """
    global last_rain_seen_ms, was_raining

    # Update last_rain_seen_ms only on valid "rain" reads
    if got_valid and dist_cm > 0 and dist_cm < RAIN_ON_CM:
        last_rain_seen_ms = now_ms

    if not got_valid:
        # Don’t drop immediately on timeout: hold if we recently saw rain
        if was_raining and time.ticks_diff(now_ms, last_rain_seen_ms) <= RAIN_HOLD_MS:
            return True
        return was_raining

    # Hysteresis switching
    if not was_raining:
        return dist_cm < RAIN_ON_CM
    else:
        if (
            dist_cm > RAIN_OFF_CM
            and time.ticks_diff(now_ms, last_rain_seen_ms) > RAIN_HOLD_MS
        ):
            return False
        return True


# -----------------------------------------
# Wiper sweep logic
# -----------------------------------------
def wiper_sweep(color_name):
    """
    Perform one full sweep and return to idle LED.
    color_name: "manual" or "auto"
    """
    global last_sweep_ms

    ilog(f"Sweep: {color_name}")

    if color_name == "manual":
        set_rgb(0, 0, 1)  # blue
    else:
        set_rgb(1, 0, 0)  # red

    set_servo_angle(LEFT_ANGLE)
    set_servo_angle(RIGHT_ANGLE)
    set_servo_angle(LEFT_ANGLE)

    set_rgb(0, 1, 0)  # back to green idle
    last_sweep_ms = time.ticks_ms()


# -----------------------------------------
# Headlight update
# -----------------------------------------
headlight_on = False
last_ldr_value = 0


def update_headlight():
    """
    Read ADC and apply your rule:
    Headlight ON when LDR < 600
    """
    global headlight_on, last_ldr_value

    # Pico ADC returns 0..65535 in MicroPython (u16), not 0..4095 like Arduino
    # IMPORTANT: your 600 threshold is super low in u16 scale.
    # If you meant "Arduino-scale 0..4095", convert like: raw12 = raw_u16 >> 4
    raw_u16 = ldr_adc.read_u16()
    raw12 = raw_u16 >> 4  # convert to ~0..4095 scale (matches your threshold logic)

    last_ldr_value = raw12

    prev = headlight_on
    headlight_on = raw12 < LDR_THRESHOLD

    if headlight_on != prev:
        ilog("Headlight -> " + ("ON" if headlight_on else "OFF"))

    headlight_led.value(1 if headlight_on else 0)


# -----------------------------------------
# Main loop
# -----------------------------------------
def main():
    global waiting_second, first_tap_us, manual_continuous_enabled
    global was_raining, last_valid_distance_cm, last_sweep_ms

    ilog("Starting MicroPython wiper system...")
    tft_init()

    # last sweep time for continuous mode spacing
    last_sweep_ms = time.ticks_ms()

    while True:
        now_ms = time.ticks_ms()
        now_us = time.ticks_us()

        # 1) Read LDR + update headlight
        update_headlight()

        # 2) Handle button events ASAP (from IRQ queue)
        #    We drain the queue so double taps don’t get stuck behind slow code.
        while True:
            tap_us = pop_button_event_us()
            if tap_us is None:
                break

            ilog(f"Tap event us={tap_us}")

            if not waiting_second:
                waiting_second = True
                first_tap_us = tap_us
                dlog("First tap -> waiting for second")
            else:
                dt = time.ticks_diff(tap_us, first_tap_us)
                if dt <= DOUBLE_TAP_WINDOW_US:
                    # DOUBLE TAP
                    waiting_second = False
                    ilog("DOUBLE TAP detected")

                    manual_continuous_enabled = not manual_continuous_enabled
                    ilog(
                        "Manual continuous -> "
                        + ("ENABLED" if manual_continuous_enabled else "DISABLED")
                    )

                    # Update display mode immediately
                    if manual_continuous_enabled:
                        set_display_mode(MODE_MANUAL_CONTINUOUS, 0)
                        # allow immediate sweep
                        last_sweep_ms = 0
                    else:
                        set_display_mode(MODE_IDLE, 0)
                else:
                    # Too late: treat as a new first tap
                    first_tap_us = tap_us
                    waiting_second = True
                    dlog("Second tap too late -> restart window")

        # 3) If we were waiting for a second tap and window expired -> SINGLE TAP
        if waiting_second:
            dt = time.ticks_diff(now_us, first_tap_us)
            if dt > DOUBLE_TAP_WINDOW_US:
                waiting_second = False
                ilog("SINGLE TAP confirmed")

                # Show MANUAL(SINGLE) on display for 1.2s so it’s visible
                set_display_mode(MODE_MANUAL_SINGLE, 1200)

                # Perform manual sweep
                wiper_sweep("manual")

        # 4) Ultrasonic (skip just after sweep to avoid servo noise)
        dist = -1.0
        got_valid = False

        if (
            last_sweep_ms
            and time.ticks_diff(now_ms, last_sweep_ms)
            < ULTRASONIC_COOLDOWN_AFTER_SWEEP_MS
        ):
            # reuse last good reading
            dist = last_valid_distance_cm
            got_valid = dist > 0
            dlog("Ultrasonic skipped (cooldown after sweep)")
        else:
            dist = measure_distance_filtered_cm()
            if dist < 0:
                wlog("Ultrasonic timeout")
                got_valid = False
            else:
                got_valid = True
                last_valid_distance_cm = dist
                dlog(f"Ultrasonic distance(cm)={dist:.2f}")

        # 5) Compute raining without flicker
        raining_now = compute_raining(
            got_valid, dist if got_valid else last_valid_distance_cm, now_ms
        )

        if raining_now != was_raining:
            ilog("Rain -> " + ("ON" if raining_now else "OFF"))
            if raining_now:
                last_sweep_ms = 0
            was_raining = raining_now

        # 6) Continuous wiping logic
        continuous_by_rain = raining_now
        continuous_by_manual = manual_continuous_enabled
        continuous_active = continuous_by_rain or continuous_by_manual

        if continuous_active:
            if continuous_by_rain and continuous_by_manual:
                set_display_mode(MODE_AUTO_PLUS_MANUAL, 0)
            elif continuous_by_rain:
                set_display_mode(MODE_AUTO_CONTINUOUS, 0)
            else:
                set_display_mode(MODE_MANUAL_CONTINUOUS, 0)

            if time.ticks_diff(now_ms, last_sweep_ms) >= CONTINUOUS_SWEEP_INTERVAL_MS:
                if continuous_by_rain:
                    wiper_sweep("auto")
                else:
                    wiper_sweep("manual")
        else:
            # If not holding a temporary mode, return to IDLE
            if (
                display_hold_until_ms == 0
                or time.ticks_diff(now_ms, display_hold_until_ms) > 0
            ):
                set_display_mode(MODE_IDLE, 0)

        # 7) Update TFT (or console fallback)
        tft_update(
            raining_now=raining_now,
            mode_str=mode_to_string(display_mode),
            head_on=headlight_on,
            ldr_value=last_ldr_value,
        )

        # small delay so we don't max out CPU; IRQ still catches taps
        time.sleep_ms(1)


# Run
main()
