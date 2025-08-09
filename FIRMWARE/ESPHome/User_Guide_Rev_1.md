# VisionGuard Rev.1 — ESPHome + ToF (VL53L3CX) + IR (LG) 

This document explains **how the configuration works (for maintainers)** and **what the end user sees/does on the device web page**.

---

## 1) What VisionGuard does (at a glance)
- Reads distance from a **VL53L3CX** ToF sensor, smoothed with **EMA** to suppress noise.
- Implements a **two‑state safety FSM**:  
  **STANDBY → (near for X s) → ACTIVE → (far/no‑object for Y s) → STANDBY**.
- When entering **ACTIVE** (child too close), it runs a configurable **“Near Action”** (e.g., **Mute**, **Power Off**, **Switch input**).  
  When returning to **STANDBY**, it runs a configurable **“Far Action”** (e.g., **Unmute**, **Power On**, **Back**).
- Sends **LG IR** codes via `remote_transmitter` on GPIO4. Actions are queued and retried with a configurable repeat count.

> Defaults: **Near = Mute**, **Far = Unmute**, **Close = 0.80 m**, **Far = 1.00 m**, **Close grace = 2 s**, **Far hold = 2 s**, **IR repeats = 3**.

---

## 2) Architecture (ESPHome maintainer view)

### 2.1 Platform
- **Board:** `esp32-c6-devkitm-1`
- **Framework:** ESP‑IDF (currently `version: latest`)
- **Web UI:** `web_server: version: 3`

### 2.2 Networking
- **Wi‑Fi:** `wifi.networks[0]` from secrets; AP fallback `'{device_name} AP'` (`password: 12345678`).
- **API/OTA:** Enabled; `api.reboot_timeout: 0s` (stays connected to HA even if idle).
- **Time:** SNTP using `Asia/Dubai` and three servers (two stratum servers + pool).

### 2.3 ToF sensing
- **Bus:** I²C on `GPIO6/7` at `38 kHz` (scanning on).
- **Sensor hub:** `vl53l3cx:` component (external_components `my_components`), device address `0x29`.
- **Measurement:** `distance_mode: MEDIUM`, `timing_budget: 80 ms`, `inter_measurement_period: 100 ms`, driver `update_interval: 200 ms`.
- **Smoothing:** `exponential_moving_average: alpha: 0.1, send_every: 4` (≈0.8–1.0 Hz publishes).
- **Entity:** `sensor.primary_target` (“Target Distance”, m, 3 decimals).

### 2.4 IR transmit
- **Transmitter:** `remote_transmitter` on **GPIO4**, `carrier_duty_percent: 50%`.  
  LG protocol frames are sent via `remote_transmitter.transmit_lg` with **repeat** block:
  `repeat.times = ir_repeat_count`, `wait_time: 50 ms` (50,000 µs).

### 2.5 State machine & timers
- `switch.guard_enabled` gates all automation.
- `globals.protection_active_state` is the current FSM state (false=STANDBY, true=ACTIVE).
- **On each distance publish (`sensor.primary_target.on_value`)**:
  - **Near region:** `d < close_threshold_m` → start **near_guard_timer** once; stop **far_hold_timer**.
  - **Far region:** `isnan(d) || d > far_threshold_m` → start **far_hold_timer** once; stop **near_guard_timer**.
  - **Deadband:** between thresholds → stop both timers (no pending fire).
- **near_guard_timer:** wait `close_grace_s`; re‑check *still near* and guard **not already ACTIVE** → set ACTIVE and `execute_near_action`.
- **far_hold_timer:** wait `far_hold_s`; re‑check *still far or no object* and guard **ACTIVE** → clear ACTIVE and `execute_far_action`.

### 2.6 User‑configurable entities
These are inputs the user can tune from the device web page or Home Assistant:

| Type | ID | Display Name | Range / Options | Default |
|---|---|---|---|---|
| `switch` | `guard_enabled` | Protection Enabled | On/Off | Off |
| `number` | `close_threshold_m` | Close Threshold (m) | 0.30–2.00 (0.05 step) | 0.80 |
| `number` | `far_threshold_m` | Far Threshold (m) | 0.50–3.00 (0.05 step) | 1.00 |
| `number` | `close_grace_s` | Close Grace (s) | 1–30 (1 step) | 2 |
| `number` | `far_hold_s` | Far Hold (s) | 1–30 (1 step) | 2 |
| `number` | `ir_repeat_count` | IR Repeat Count | 1–5 (1 step) | 3 |
| `select` | `near_action` | Near Action (Child Too Close) | *Mute, Power Off, Switch to HDMI1/2/3, Switch to TV, Home Dashboard, Show Input List, Exit/Back, Do Nothing* | Mute |
| `select` | `far_action` | Far Action (Child Moved Away) | *Unmute, Power On, Switch to Previous, Switch to HDMI1/2/3, Switch to TV, Home Dashboard, Exit/Back, Do Nothing* | Unmute |

Additional telemetry:
- `sensor.uptime`, `sensor.internal_temperature`, `sensor.wifi_signal`
- `text_sensor.wifi_info.ip_address`

### 2.7 Buttons (for testing/ops)
- **Test Near Action** → runs `execute_near_action` immediately (ignores distance).
- **Test Far Action** → runs `execute_far_action` immediately.
- **Force Mute** → sends LG Mute via the action mapper (queued).
- **Force Input** → opens LG input list via the action mapper (queued).

### 2.8 IR action mapping
`script.send_lg_command_mapped` translates a selected action to one of these LG commands (all are emitted by `script.send_lg_ir` with repeats):

| Action(s) | LG Command | Notes |
|---|---|---|
| Power On / Power Off | `power` | **Toggle code** (0x20DF10EF). If you need discrete On/Off, the TV must support them; otherwise this toggles. |
| Mute / Unmute | `mute` | Toggle mute (0x20DF906F). |
| Show Input List | `input` | 0x20DFD02F. |
| Switch to HDMI1 / 2 / 3 | `hdmi1` / `hdmi2` / `hdmi3` | 0x20DF738C / 0x20DF33CC / 0x20DF9966. |
| Switch to TV (Tuner) | `tv` | 0x20DF0FF0. |
| Home Dashboard | `home` | 0x20DFDA25. |
| Exit/Back / Switch to Previous | `back` | 0x20DF14EB. |

> The **repeat count** multiplies the command reliability on fussy sets. Start at 2–3; increase only if needed.

### 2.9 Boot‑time safety
On boot, if `close_threshold_m >= far_threshold_m`, the config nudges `far_threshold_m` up by +0.10 m and logs a warning.

---

## 3) End‑user guide (what you see on the device web page)

> Open a browser to the device’s IP (shown on the page as **IP Address**). The UI is the standard ESPHome Web Server v3.

### Main controls
1. **Protection Enabled (switch):** turn the safety automation on/off. When Off, the system won’t trigger any actions.
2. **Target Distance (sensor):** live distance in meters (smoothed). Use this to calibrate thresholds.
3. **Near/Far Thresholds:** set the “too close” and “safe” distances. Keep **Far > Close**. A dead‑band between them prevents chattering.
4. **Close Grace / Far Hold:** how long to wait *continuously* in each zone before acting.
5. **Near Action / Far Action:** pick what happens when entering/leaving the “too close” state.
6. **IR Repeat Count:** number of protocol‑level repeats for each IR send.
7. **Test Buttons:** try actions without moving in front of the sensor (useful for setup).

### Typical setup flow
1. **Connect Wi‑Fi** (first boot may expose an AP `VisionGuard Rev.1 AP`). Then open the device web page.
2. **Enable Protection.**
3. **Stand at the desired minimum distance** and read *Target Distance*. Set **Close Threshold** a bit higher than that value (e.g., add 0.05–0.10 m).
4. **Walk back to the “safe” distance** you want. Set **Far Threshold** slightly beyond that.
5. Choose **Near Action** (e.g., *Mute* or *Power Off*) and **Far Action** (e.g., *Unmute* or *Power On*).
6. Set **Close Grace** (~2–3 s) and **Far Hold** (~2–3 s) to avoid nuisance triggers.
7. Use **Test Near/Far Action** to verify the TV reacts. If IR is intermittent, raise **IR Repeat Count** to 3–4.
8. Confirm that **leaving the field of view counts as Far** (this build treats “no object” as Far by design).

### Status & diagnostics
- The page also shows **Uptime**, **MCU Temperature**, **Wi‑Fi Signal**, and **IP Address**.
- The device logs status lines every 60 s (e.g., *Protection ON – Distance: 0.75 m* or *Protection ON – No object detected*), plus action traces.
- If needed, use the **OTA** tab (via ESPHome/HA) for upgrades and the **log viewer** for live debugging.

---

## 4) Customization & maintenance notes

- **LG‑only IR by default.** To support another brand, replace the `transmit_lg` blocks with the appropriate protocol (e.g., `transmit_nec`) and codes, keeping the same mapper structure.
- **Carrier tuning.** If certain TVs are picky, you can add `frequency: 38kHz` under `remote_transmitter` to match common LG remotes.
- **EMA cadence.** `send_every: 4` keeps automation responsive while filtering noise. If your scene is jittery, try `alpha: 0.08–0.12` before increasing `send_every`.
- **Deadband behavior.** The automation cancels pending timers in the deadband to avoid stale fires. If you want “latch on first entry,” move the cancelation to the opposite edge; otherwise keep as‑is.
- **No‑object semantics.** This build treats **no object as FAR** for the “move away” action. To change that, edit the FAR condition and post‑delay check to drop `isnan(d)`.

---

## 5) Troubleshooting

- **Actions don’t fire:** Ensure **Protection Enabled** is ON. Check that **Close < Far** (the device will auto‑nudge on boot if not). Watch the logs for “Executing Near/Far Action…” lines.
- **IR unreliable:** Increase **IR Repeat Count**; ensure clear line‑of‑sight to the TV’s IR receiver; consider adding `frequency: 38kHz` under `remote_transmitter`.
- **Triggers feel laggy:** Reduce **Close Grace / Far Hold** or lower `send_every` (more frequent publishes). Don’t remove EMA entirely; noise will cause oscillations.
- **Too sensitive near threshold:** Raise **Close Threshold** or increase **Close Grace** by 1–2 s.
- **Not unmuting when the child leaves FOV:** This build already counts **no object** as FAR. If behavior differs, check the logs to ensure the sensor actually goes to “no object” (NaN).

---

## 6) Files & IDs (quick reference)
- Device name: **VisionGuard Rev.1** (`vision-guard-rev-1`)
- Key IDs: `primary_target`, `guard_enabled`, `protection_active_state`, `near_guard_timer`, `far_hold_timer`, `execute_near_action`, `execute_far_action`, `send_lg_command_mapped`, `send_lg_ir`.

—

*Maintainer tip:* keep a copy of the IR code table in comments next to the mapper. If you add new actions, extend `select` options and update `send_lg_command_mapped` to route them.