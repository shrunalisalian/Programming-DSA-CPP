# Apple MLE — On-Device Control & Optimization
## 100 Hard Interview Questions + Complete Solutions

**Role:** Machine Learning Engineer - On-Device Control and Optimization (Energy Tech, Seattle)  
**Focus:** Power/energy tradeoffs, device dynamics modeling, MPC/RL control, thermal/battery, on-device ML, raw log analysis  
**Informed by:** Past Apple interviews (running median, sensor fusion, gesture event extraction, document retrieval, encrypted pipelines, parallel log processing)

---

# PART 1: CODING & DATA STRUCTURES (15 Questions)

## Q1: Running Median of Thermal Sensor Stream
**Problem:** A device streams junction temperature readings at 10 Hz. Implement `addReading(t)` and `getMedian()` returning the median of all readings so far. Follow-up: bound memory if the stream runs for days.

**Solution:**
Use two heaps (max-heap for lower half, min-heap for upper half). `addReading`: O(log n). `getMedian`: O(1).

```python
import heapq

class ThermalMedian:
    def __init__(self, max_samples=None):
        self.low = []   # max-heap via negation
        self.high = []  # min-heap
        self.max_samples = max_samples
        self.count = 0

    def addReading(self, t: float) -> None:
        if not self.low or t <= -self.low[0]:
            heapq.heappush(self.low, -t)
        else:
            heapq.heappush(self.high, t)
        if len(self.low) > len(self.high) + 1:
            heapq.heappush(self.high, -heapq.heappop(self.low))
        elif len(self.high) > len(self.low):
            heapq.heappush(self.low, -heapq.heappop(self.high))
        self.count += 1
        if self.max_samples and self.count > self.max_samples:
            self._evict_oldest()  # use ring buffer + periodic reheap or count-based buckets

    def getMedian(self) -> float:
        if len(self.low) > len(self.high):
            return float(-self.low[0])
        return (-self.low[0] + self.high[0]) / 2.0
```

**Memory bound:** For infinite streams, switch to **time-windowed median** (e.g., last 3600 samples) or **t-digest / Greenwald-Khanna** for approximate median in O(1) space. Apple interview follow-up expects you to mention both exact (heap + window) and approximate (streaming quantile sketch).

---

## Q2: Power Event Index — DocumentStore Pattern
**Problem:** Build a system supporting: (1) `add_session(session_id, log_text)` where log contains workload tags, (2) `query(workload_tag)` returning the session_id where that tag's cumulative power (mW·s) is highest. Return -1 if not found.

**Solution:** Inverted index: `tag → {session_id: total_power}`.

```python
import re
from collections import defaultdict

class PowerLogStore:
    def __init__(self):
        self.index = defaultdict(lambda: defaultdict(float))

    def add_session(self, session_id: int, log_text: str) -> None:
        # Format: "video_decode 120mW 2s, idle 30mW 5s"
        for match in re.finditer(r"(\w+)\s+(\d+)mW\s+(\d+)s", log_text):
            tag, mw, sec = match.group(1), int(match.group(2)), int(match.group(3))
            self.index[tag][session_id] += mw * sec

    def query(self, tag: str) -> int:
        if tag not in self.index or not self.index[tag]:
            return -1
        return max(self.index[tag], key=self.index[tag].get)
```

**Example:** Session 1: `"video 200mW 3s"`, Session 2: `"video 150mW 5s"` → query `"video"` returns 2 (750 vs 600 mW·s). Same pattern as Apple IR team DocumentStore question.

---

## Q3: Extract Throttling Events from Time Series
**Problem:** Given CPU frequency samples every 125 ms and ground-truth throttling intervals `[{start, end}]`, write `extract_events(predictions_df)` and `evaluate(gt, pred)` with precision/recall on interval IoU ≥ 0.5.

**Solution:** Mirror Apple SDE-QA gesture extraction pattern.

```python
import pandas as pd

def extract_events(df, freq_col="cpu_freq_mhz", threshold=1200):
    events, in_evt, start = [], False, None
    for i, row in df.iterrows():
        throttled = row[freq_col] < threshold
        t = row["timestamp_s"]
        if throttled and not in_evt:
            in_evt, start = True, t
        elif not throttled and in_evt:
            events.append({"start": start, "end": t})
            in_evt = False
    if in_evt:
        events.append({"start": start, "end": df.iloc[-1]["timestamp_s"]})
    return events

def interval_iou(a, b):
    inter = max(0, min(a["end"], b["end"]) - max(a["start"], b["start"]))
    union = max(a["end"], b["end"]) - min(a["start"], b["start"])
    return inter / union if union > 0 else 0

def evaluate(gt, pred, iou_thresh=0.5):
    tp = sum(1 for g in gt if any(interval_iou(g, p) >= iou_thresh for p in pred))
    fp = len(pred) - tp
    fn = len(gt) - tp
    prec = tp / (tp + fp) if tp + fp else 0
    rec = tp / (tp + fn) if tp + fn else 0
    return {"precision": prec, "recall": rec, "f1": 2*prec*rec/(prec+rec) if prec+rec else 0}
```

---

## Q4: Parallel Device Log Word Count (Google Pattern)
**Problem:** `countPowerSamples(machine_id, log_id) -> Future[int]`. Given more machines than logs, count total power samples across all logs minimizing wall-clock time.

**Solution:** Assign one future per log; machines exceed logs so parallelize per document.

```python
def total_samples(machine_ids, log_ids, countPowerSamples):
    futures = {}
    for i, log_id in enumerate(log_ids):
        mid = machine_ids[i % len(machine_ids)]
        futures[log_id] = countPowerSamples(mid, log_id)
    return sum(f.join() for f in futures.values())
```

**Complexity:** O(num_logs × latency_per_job) wall time vs O(num_logs × num_machines) sequential.

---

## Q5: Power State Task Scheduler
**Problem:** Tasks have `[start, end, power_mw]`. Device budget is 5000 mW. Schedule maximum tasks without exceeding budget at any time (LeetCode 253 + resource variant).

**Solution:** Sort by start; min-heap on end times; track concurrent power.

```python
import heapq

def max_tasks(events, budget_mw):
    events.sort(key=lambda x: x[0])
    active = []  # (end, power)
    power_used = 0
    count = 0
    for start, end, p in events:
        while active and active[0][0] <= start:
            _, pw = heapq.heappop(active)
            power_used -= pw
        if power_used + p <= budget_mw:
            heapq.heappush(active, (end, p))
            power_used += p
            count += 1
    return count
```

---

## Q6: Sliding Window Maximum CPU Utilization
**Problem:** Return max CPU % in every window of size k over a stream.

**Solution:** Monotonic deque — O(n) total.

```python
from collections import deque

def sliding_max(arr, k):
    dq, out = deque(), []
    for i, v in enumerate(arr):
        while dq and arr[dq[-1]] <= v:
            dq.pop()
        dq.append(i)
        if dq[0] <= i - k:
            dq.popleft()
        if i >= k - 1:
            out.append(arr[dq[0]])
    return out
```

---

## Q7: Merge Overlapping High-Power Sessions
**Problem:** Intervals `[start_mw_time, end]` of sessions above 8W. Merge overlaps.

**Solution:** Sort by start, merge if `curr.start <= prev.end`.

```python
def merge_intervals(intervals):
    if not intervals:
        return []
    intervals.sort()
    merged = [intervals[0]]
    for s, e in intervals[1:]:
        if s <= merged[-1][1]:
            merged[-1][1] = max(merged[-1][1], e)
        else:
            merged.append([s, e])
    return merged
```

---

## Q8: Encrypted Sensor Fusion Pipeline
**Problem:** Accelerometer, gyro, and power meter readings arrive encrypted. Decrypt → fuse → compute composite motion-power score → re-encrypt result. Design the pipeline.

**Solution:**
```python
def pipeline(enc_accel, enc_gyro, enc_power, decrypt, encrypt, fuse):
    a, g, p = decrypt(enc_accel), decrypt(enc_gyro), decrypt(enc_power)
    # Reliability-weighted fusion (accel for motion, gyro for rotation, power validates activity)
    w_a, w_g = (0.7, 0.3) if abs(a) > 0.5 else (0.3, 0.7)
    motion = w_a * a + w_g * g
    score = motion * (1 + 0.01 * p)  # higher draw during motion
    return encrypt(score)
```
**Key points:** Never persist decrypted values; zeroize buffers; batch decrypt only in secure enclave if available; same pattern as Nuro encrypted sensor fusion interview.

---

## Q9: Valid Power State Transition DAG
**Problem:** Power states: OFF → SLEEP → IDLE → ACTIVE → BOOST. Some transitions forbidden. Given n transition requests `[from, to]`, return whether all can be scheduled (Course Schedule variant).

**Solution:** Build graph of legal transitions; BFS/DFS cycle detection on requested sequence or topological sort for batch validation.

```python
LEGAL = {("OFF","SLEEP"), ("SLEEP","IDLE"), ("IDLE","ACTIVE"), ("ACTIVE","BOOST"),
         ("BOOST","ACTIVE"), ("ACTIVE","IDLE"), ("IDLE","SLEEP"), ("SLEEP","OFF")}

def valid_sequence(requests):
    for a, b in requests:
        if (a, b) not in LEGAL:
            return False
    return True
```

---

## Q10: Running P95 Inference Latency
**Problem:** Maintain approximate 95th percentile of on-device inference latency with bounded memory.

**Solution:** Two heaps for median + **t-digest** or bucket histogram (100 buckets over [0, 200ms]). For interviews, describe Greenwald-Khanna sketch: ε-approximate quantile in O(1/ε log(εn)) space.

```python
# Simplified histogram approach
class P95Tracker:
    def __init__(self, bins=50, max_ms=200):
        self.bins, self.max_ms = bins, max_ms
        self.counts = [0] * bins
        self.n = 0

    def add(self, ms):
        idx = min(int(ms / self.max_ms * self.bins), self.bins - 1)
        self.counts[idx] += 1
        self.n += 1

    def p95(self):
        target = int(0.95 * self.n)
        cum = 0
        for i, c in enumerate(self.counts):
            cum += c
            if cum >= target:
                return (i + 0.5) / self.bins * self.max_ms
        return self.max_ms
```

---

## Q11: Binary Search on Battery Discharge Curve
**Problem:** Given monotonic SOC vs voltage curve, find SOC for measured voltage V in O(log n).

**Solution:** Binary search on sorted voltage array; interpolate linearly between bracketing points.

```python
def soc_from_voltage(voltages, socs, v):
    lo, hi = 0, len(voltages) - 1
    while lo < hi:
        mid = (lo + hi) // 2
        if voltages[mid] < v:
            lo = mid + 1
        else:
            hi = mid
    if lo == 0:
        return socs[0]
    t = (v - voltages[lo-1]) / (voltages[lo] - voltages[lo-1])
    return socs[lo-1] + t * (socs[lo] - socs[lo-1])
```

---

## Q12: Thermal RC Network Matrix Multiply
**Problem:** 3-node thermal model: `T_{t+1} = A @ T_t + B @ P_t`. Implement one simulation step for given A (3×3), B (3×1), state T, power vector P.

**Solution:**
```python
import numpy as np
def thermal_step(T, P, A, B, dt=1.0):
    # Euler: T' = A T + B P  (discretized)
    return T + dt * (A @ T + B @ P)
```
**Follow-up:** For on-device, precompute `(I + dtA)` and use fixed-point INT16 for T to avoid float cost.

---

## Q13: Segment Tree for Range-Max Power Queries
**Problem:** Static array of per-ms power samples (n=1e6). Answer q range-max queries offline/online.

**Solution:** Segment tree O(n) build, O(log n) query; or sparse table O(n log n) preprocess, O(1) query if immutable.

---

## Q14: Anomaly Detection in Current Draw
**Problem:** Detect spikes >3σ above rolling mean in O(1) amortized per sample.

**Solution:** Welford's online variance + ring buffer mean, or Z-score on exponential moving average.

```python
class SpikeDetector:
    def __init__(self, alpha=0.01, k=3):
        self.mu = self.var = 0
        self.alpha, self.k = alpha, k
        self.n = 0

    def update(self, x):
        self.n += 1
        delta = x - self.mu
        self.mu += self.alpha * delta
        self.var = (1 - self.alpha) * (self.var + self.alpha * delta * delta)
        sigma = self.var ** 0.5
        return x > self.mu + self.k * sigma
```

---

## Q15: Longest Subarray Under Power Budget
**Problem:** Longest contiguous window where sum(power_samples) ≤ budget.

**Solution:** Sliding window two pointers — O(n).

```python
def longest_under_budget(power, budget):
    best = left = cur = 0
    for right, p in enumerate(power):
        cur += p
        while cur > budget:
            cur -= power[left]
            left += 1
        best = max(best, right - left + 1)
    return best
```

---

# PART 2: POWER & ENERGY MODELING (15 Questions)

## Q16: First-Order Battery SOC Model
**Problem:** Derive discrete-time SOC update from current measurements.

**Solution:** Coulomb counting: `SOC_{k+1} = SOC_k - (η · I_k · Δt) / Q_max`. η = coulombic efficiency (~1.0 discharge, <1.0 charge). Handle SOC ∈ [0,1] with projection/clamping. Uncertainty grows without voltage feedback → fuse with OCV-SOC curve (Q11).

---

## Q17: Linear Power Model from Telemetry
**Problem:** Fit P = β₀ + β₁·CPU + β₂·GPU + β₃·brightness from logs.

**Solution:** Ridge regression (multicollinearity between CPU/GPU). Feature engineering: add CPU² for nonlinearity, interaction CPU×GPU. Evaluate with MAPE on holdout devices (not random split — **device-level split** to test generalization).

```python
from sklearn.linear_model import Ridge
from sklearn.metrics import mean_absolute_percentage_error
model = Ridge(alpha=1.0).fit(X_train, y_train)
# Report MAPE per device family (iPhone 15 vs 16)
```

---

## Q18: System Identification from Step Response
**Problem:** Step CPU frequency 1→3 GHz; observe power ramp. Identify first-order time constant τ.

**Solution:** Fit `P(t) = P_ss - (P_ss - P_0) exp(-t/τ)` via nonlinear least squares. τ reveals thermal/electrical inertia. For MIMO, use subspace ID (N4SID) on state-space logs.

---

## Q19: Hidden Thermal States
**Problem:** You observe skin temperature; junction temp is hidden. Model?

**Solution:** State-space: `x = [T_junction, T_skin]`, observation `y = T_skin + noise`. Kalman filter predicts junction from skin + power input. Enables proactive throttling before skin limit.

---

## Q20: Kalman Filter for SOC Estimation
**Problem:** Fuse Coulomb counting (drift) with voltage measurements (noisy at load).

**Solution:** State x=SOC; predict with current; update with voltage via OCV(SOC) Jacobian. Process noise Q models sensor drift; measurement noise R high during load transients — **gating**: skip voltage update when |dI/dt| > threshold.

---

## Q21: Energy Attribution per App
**Problem:** Attribute total energy to apps without per-app power meters.

**Solution:** Use CPU time × frequency P-states, GPU counters, network bytes × coeff, display pixels × brightness. Train ML attribution model on lab ground truth (Monsoon power monitor + syspower). On-device: lightweight linear model with features from `os_log` counters.

---

## Q22: Predict Display Power from APL
**Problem:** Model OLED power vs average picture level (APL) and nits.

**Solution:** `P_display ≈ α·APL·brightness² + β·refresh_rate`. OLED emits per-pixel; APL captures content. Validate on dark mode vs light mode; add HDR compensation term.

---

## Q23: Radio Power States (RRC)
**Problem:** LTE modem has IDLE, DRX, CONNECTED. Model energy of burst upload.

**Solution:** State machine energy = Σ (time_in_state × power_state) + transition penalties. Tail energy after upload dominates — **batch uploads** to amortize tail. ML predicts optimal batch window from traffic pattern.

---

## Q24: Leakage vs Dynamic Power
**Problem:** At low load, leakage dominates. How does this affect DVFS?

**Solution:** P_total = P_dyn + P_leak ∝ CV²f + I_leak(V,T). Below f_min, leakage fraction rises — **race-to-idle** vs **stay-slow** depends on task length L: choose f that minimizes E = P(f)·L/f. Critical task length: `L* ≈ (P_dyn(f_max)-P_dyn(f_min)) / (P_leak(f_min)-P_leak(f_max))`.

---

## Q25: Power Model Transfer Across Devices
**Problem:** Model trained on iPhone 15; deploy on iPhone 16 with different silicon.

**Solution:** Domain adaptation: scale coefficients per chip ID; few-shot calibration from first 24h user data (on-device ridge update); hierarchical model with device-family random effects.

---

## Q26: Causal vs Correlational Features in Power Logs
**Problem:** Screen-on correlates with high power — is it causal?

**Solution:** Use interventional data (AB tests on brightness policies) or instrumental variables. Avoid policy learning on confounded logs (games → high GPU + high brightness).

---

## Q27: Estimate Q_max Degradation (SOH)
**Problem:** Battery ages; Q_max drops. Detect from field data.

**Solution:** Track integrated current over 100→0% cycles; compare to nominal. ML: features = [cycle_count, avg_temp, avg_DOD, charge_rate]; label = SOH from lab cells. Ship tiny regression on-device for personalized SOC scaling.

---

## Q28: Workload Classification from Power Signature
**Problem:** Classify {gaming, video, idle, navigation} from 1s power trace.

**Solution:** 1D-CNN or hand-crafted features (variance, spectral entropy, peak power). Quantize to INT8 for on-device; 50ms window, 90% accuracy target with <1mJ/inference.

---

## Q29: Energy Cost Function Design
**Problem:** Define scalar cost J combining UX latency and energy.

**Solution:** `J = w₁·E_mJ + w₂·max(0, latency - L_SLO)² + w₃·thermal_violation`. Weights encode product priorities; Pareto frontier across w for stakeholder choice. Hard constraints via barrier methods in MPC.

---

## Q30: Field vs Lab Distribution Shift
**Problem:** Lab power traces don't match field.

**Solution:** Importance reweighting by ambient temp, case usage, cellular vs WiFi ratio. Continuous monitoring: KL divergence on feature distributions triggers recalibration alert.

---

# PART 3: CONTROL SYSTEMS — MPC, OPTIMAL, RL (15 Questions)

## Q31: MPC for Thermal Throttling
**Problem:** Control CPU frequency u_k to keep T_k ≤ T_max while maximizing performance.

**Solution:** Receding horizon: minimize `Σ (T_k - T_ref)² + λ·perf_loss(u_k)` subject to thermal dynamics `T_{k+1} = aT_k + b·u_k²`. Solve QP each 100ms with horizon H=10. On-device: precompute explicit MPC lookup table for 2D (T, workload) state.

---

## Q32: PID for Fan/Throttle Control
**Problem:** Tune PID where thermal plant has delay.

**Solution:** Ziegler-Nichols or relay feedback for ultimate gain. Add anti-windup on integrator when saturated at f_min. Derivative filter: `d_filtered = α·d + (1-α)·d_prev` to reduce noise. Delay → reduce K_d or add Smith predictor.

---

## Q33: Bang-Bang vs Smooth Control
**Problem:** Why not simply ON/OFF throttle?

**Solution:** Bang-bang causes limit cycles (oscillation around setpoint), bad for UX (frame drops). Hysteresis band + rate limits on u_k smooth output. ML learns smooth policy via imitation of MPC.

---

## Q34: LQR for Power-State Tracking
**Problem:** Linearized state `[SOC, T_junction]`; control `u = [f_cpu, brightness]`. Design LQR.

**Solution:** Solve Riccati for K in `u = -Kx`. Linearize at operating point; verify stability for ±20% parameter variation. On-device: store K as fixed matrix multiply.

---

## Q35: Constraint Handling in MPC
**Problem:** Hard constraints on SOC > 5%, T < 85°C.

**Solution:** Formulate as QP with inequality constraints Gx ≤ h. Active-set or interior point; for embedded use **explicit MPC** — partition state space, store affine control law per region.

---

## Q36: RL for DVFS — State/Action/Reward
**Problem:** Design MDP for on-device DVFS.

**Solution:** State: [CPU util, T, SOC, workload_class]. Action: discrete P-states. Reward: `-(energy_mJ + α·latency_ms + β·thermal_penalty)`. Use constrained RL (CPO) for hard thermal limits. Offline RL from logs (no exploration on user devices).

---

## Q37: Sim-to-Real for Control Policies
**Problem:** RL policy works in simulator, fails on device.

**Solution:** Domain randomization (R, C, T_amb); system ID to fit sim; fine-tune with on-device safe policy improvement using logged data only.

---

## Q38: Hierarchical Control Architecture
**Problem:** Separate timescales for energy (slow) and thermal (fast).

**Solution:** Inner loop 10Hz thermal MPC; outer loop 0.1Hz energy budget allocator across subsystems. Avoid coupling instability via separation principle when timescales differ 10×.

---

## Q39: Reference Governor
**Problem:** User demands max performance but device hot.

**Solution:** Reference governor modifies desired perf r to r_safe such that constraints satisfied forward in time. Implement as pre-filter before MPC.

---

## Q40: Delay Compensation
**Problem:** Sensor-to-actuator delay 50ms.

**Solution:** Model delay in state augmentation; predict T_{k+delay} for control decision. Smith predictor or MPC with delayed state estimate.

---

## Q41: Multi-Objective Pareto Control
**Problem:** Can't minimize energy and latency simultaneously.

**Solution:** Scalarize with weights (Q29) or track Pareto set offline; on-device switch weights by Low Power Mode flag.

---

## Q42: Robust MPC Under Model Error
**Solution:** Min-max MPC or tube MPC — tighten constraints by uncertainty bound ε. Critical when thermal R,C vary ±30% across units.

---

## Q43: Discrete vs Continuous Action Spaces
**Problem:** Apple P-states are discrete; MPC outputs continuous.

**Solution:** Round with hysteresis; or formulate as MIQP (expensive). Learn Q-table over discrete actions for embedded.

---

## Q44: Stability of Learned Controllers
**Problem:** Neural network policy — prove stability?

**Solution:** Lyapunov certificate post-training; or constrain policy to be Lipschitz with bounded gain; fallback to PID if NN output exceeds safe envelope.

---

## Q45: Control Loop Timing Jitter
**Problem:** MPC should run every 100ms but OS jitter causes 80–150ms.

**Solution:** Timestamp-based state prediction to next nominal tick; WCET analysis for solver; reduce horizon if overrun detected.

---

# PART 4: THERMAL & BATTERY MANAGEMENT (12 Questions)

## Q46: Skin vs Junction Temperature Limit
**Problem:** Why throttle at junction when user feels skin temp?

**Solution:** Junction hits limit first on burst; skin lags (thermal resistance). Control on junction with model predicting skin; UX constraint on skin T as separate output constraint.

---

## Q47: Thermal Inertia and Burst Workloads
**Problem:** 30s game burst — when to start throttling?

**Solution:** Predictive: if `T + τ·P > T_max` forecast, preemptive downclock. MPC handles naturally; reactive-only lags → overshoot.

---

## Q48: Ambient Temperature Compensation
**Problem:** Same workload, different T_amb (Phoenix vs Seattle).

**Solution:** Augment state with T_amb estimate (slow low-pass from idle periods); shift throttle curve. ML regressor on {workload, T_amb} → f_max allowed.

---

## Q49: Charging While Gaming
**Problem:** Simultaneous high charge current and GPU load.

**Solution:** Coupled thermal/electrical budget: `I_charge + I_compute ≤ I_batt_max(T)`. Joint optimizer reduces charge rate when T high — **power sharing** negotiation in firmware.

---

## Q50: Fast Charge Thermal Limits
**Problem:** 30W charge throttles at 40°C.

**Solution:** PI controller on charge current with T feedback; CCCV with thermal foldback curve from battery vendor spec.

---

## Q51: Battery Impedance Rise (Cold)
**Problem:** Low SOC + cold → voltage sag under load.

**Solution:** Predict V_terminal = OCV(SOC) - I·R(T,SOC); limit I when V near cutoff. On-device table lookup for R(T).

---

## Q52: Calendar vs Cycle Aging
**Problem:** Model aging for idle devices.

**Solution:** Features: time, T_storage, SOC_storage (avoid 100% storage); Arrhenius temperature acceleration in label generation from accelerated aging lab.

---

## Q53: Thermal Throttling Hysteresis
**Problem:** Prevent oscillation at threshold.

**Solution:** Enter throttle at 85°C, exit at 80°C. State machine in firmware; ML only sets dynamic thresholds per workload.

---

## Q54: Heat Spreader vs Vapor Chamber
**Problem:** Different transient response — impact on control?

**Solution:** Larger thermal mass → longer τ, allows shorter bursts. Re-identify τ per hardware SKU; store in device tree for controller gains.

---

## Q55: User-Perceived Performance Under Throttle
**Problem:** Metric for UX during throttle?

**Solution:** Frame time P99, touch latency, audio glitch rate — not just average FPS. Energy team optimizes constrained UX SLO.

---

## Q56: Watch vs iPhone Thermal Budget
**Problem:** Same algorithm, different constraints.

**Solution:** Parameterize by thermal mass, surface area, battery size. Auto-tune from chip ID + form factor enum.

---

## Q57: Emergency Shutdown Prediction
**Problem:** Predict hard shutdown before it happens.

**Solution:** Binary classifier on {T, dT/dt, P, SOC} → P(shutdown in 5s). Trigger graceful degradation cascade.

---

# PART 5: ON-DEVICE ML DEPLOYMENT (12 Questions)

## Q58: INT8 Quantization for Control Model
**Problem:** FP32 thermal predictor loses 2°C RMSE after INT8.

**Solution:** QAT with representative thermal trajectories; per-channel scales; keep bias in FP32; sensitive layers (final regression head) in FP16.

---

## Q59: Core ML Export for Control Loop
**Problem:** 5ms PyTorch → 12ms Core ML.

**Solution:** mlprogram format, `computeUnits = .cpuAndNeuralEngine`, fuse ops, reduce dynamic shapes. Profile with Instruments → Core ML.

---

## Q60: Model Size Budget for Daemon
**Problem:** Energy daemon limited to 500KB RAM.

**Solution:** 200KB weights (INT8 MLP), 100KB buffers, 200KB OS overhead. Distill 3-layer MLP; avoid RNN if possible (use FIR features).

---

## Q61: Feature Pipeline on Device
**Problem:** Compute 50 features from 1kHz IMU without cloud.

**Solution:** Streaming: incremental mean/var, IIR filters precomputed coeffs, downsample early. vDSP/Accelerate for vector ops.

---

## Q62: Online Learning for Personalization
**Problem:** Adapt power model per user without draining battery.

**Solution:** Federated or local SGD on idle+charging only; max 30s/day training budget; differential privacy on weight deltas.

---

## Q63: A/B Test On-Device Control Policy
**Problem:** Compare policy A vs B ethically.

**Solution:** Deterministic hash(device_id) → cohort; log aggregated metrics only; kill switch via remote config; guardrail metrics (crash, shutdown rate).

---

## Q64: OTA Model Update Rollback
**Problem:** Bad model shipped.

**Solution:** Dual partition models; shadow mode evaluation 48h; auto-rollback if thermal violation rate ↑ 2σ.

---

## Q65: Neural Engine vs CPU for Tiny MLP
**Problem:** 3-layer 64-unit MLP — where to run?

**Solution:** NE wins >1M MACs; tiny models CPU faster due to dispatch overhead. Benchmark crossover ~500K MACs on A17.

---

## Q66: Fixed-Point Implementation
**Problem:** No FPU on sensor hub.

**Solution:** Q15 format; simulate overflow; scale layers so activations ∈ [-1,1]. Validate against FP32 on HIL bench.

---

## Q67: Watchdog for ML Controller
**Problem:** Model outputs NaN frequency.

**Solution:** Output clamp [f_min, f_max]; NaN → revert to PID fallback; log counter for telemetry.

---

## Q68: Privacy in Power Logs
**Problem:** Power trace reveals app usage.

**Solution:** On-device aggregation only; differential privacy for federated updates; no raw trace upload without consent.

---

## Q69: Compiler Optimizations for Inference
**Solution:** Operator fusion, memory planning, static shapes, thread affinity to E-cores for background inference.

---

# PART 6: DATA ANALYSIS & FIELD LOGS (12 Questions)

## Q70: Parse os_log Power Entries
**Problem:** TB-scale logs with inconsistent schemas across OS versions.

**Solution:** Schema registry; defensive parsing with fallbacks; Spark/DuckDB for offline; on-device protobuf binary logs for structured capture.

---

## Q71: Missing Sensor Data Imputation
**Problem:** 30% gyro samples missing in field log.

**Solution:** Forward-fill max 3 samples; mark missingness indicator feature; model uncertainty ↑ when imputed fraction high.

---

## Q72: Change Point Detection in Power
**Problem:** Detect OS regression causing +10% drain.

**Solution:** CUSUM or Bayesian change point on daily E/device; segment by build version; alert if posterior P(change) > 0.95.

---

## Q73: Cohort Analysis for Battery Complaints
**Problem:** Spike in support tickets — find root cause.

**Solution:** Join tickets with {device, OS, temp zone, app version}; survival analysis on "unexpected shutdown"; SHAP on classifier predicting complaint.

---

## Q74: Time Zone Sync in Global Logs
**Solution:** Store UTC + monotonic clock; align events cross-sensor via interpolation to unified timeline.

---

## Q75: Sampling Bias in Opt-In Diagnostics
**Problem:** Power users opt in more.

**Solution:** Inverse propensity weighting; compare to mandatory quality metrics subset.

---

## Q76: Pandas: Resample Irregular Power Samples
```python
df = df.set_index("ts").resample("100ms").mean().interpolate(limit=3)
```

---

## Q77: Detect Idle vs Active from Power Only
**Solution:** HMM with states {sleep, idle, active}; EM on power observations; Viterbi decode.

---

## Q78: Join Lab Trace with Field Trace
**Solution:** Dynamic time warping on normalized power shape; match workload segments; transfer labels lab→field.

---

## Q79: Log Compression for Upload
**Solution:** Delta-encode timestamps, varint, zlib; 10× reduction; on-device ring buffer flush on WiFi+charging.

---

## Q80: Reproducible Analysis Pipeline
**Solution:** DVC for data; pinned pandas/numpy; Makefile with hash-verified outputs for cross-team review.

---

## Q81: Visualization for Stakeholders
**Solution:** Pareto curves (energy vs latency); thermal violation rate vs policy version; avoid raw trace dumps.

---

# PART 7: SYSTEM DESIGN (10 Questions)

## Q82: End-to-End On-Device Energy Controller
**Problem:** Design system from sensors to actuators.

**Solution:**
```
Sensors (T, I, V, counters) → Estimator (KF) → MPC/RL Policy → Actuators (DVFS, display, radio)
         ↑________________________ feedback _________________________|
```
Daemon on E-core, 10Hz loop, 2MB RAM cap, failsafe PID overlay, privacy-preserving telemetry.

---

## Q83: Design Siri Head Gesture Energy Budget (Apple CoreMotion Pattern)
**Problem:** Head gestures for Siri on AirPods + iPhone — energy constraints.

**Solution:** IMU on AirPods at 50Hz not 200Hz; on-device classifier INT8; duty-cycle when not in listening mode; HMM debounce; <0.5% daily battery; fusion with voice activity detector to gate inference.

---

## Q84: Low Power Mode Architecture
**Solution:** Global constraint propagates: reduce f_max, cap brightness, defer background ML, lengthen MPC horizon for smoother/low-power trajectory.

---

## Q85: Cross-Subsystem Negotiation
**Solution:** Power broker IPC: camera requests 2W; thermal headroom 1.5W → broker reduces CPU boost or denies camera 4K.

---

## Q86: Simulation Infrastructure
**Solution:** Digital twin per device SKU; replay field traces; HIL with Monsoon; CI regression on energy + thermal metrics per PR.

---

## Q87: Firmware ↔ ML Team Interface
**Solution:** Frozen feature contract; versioned model bundle; A/B flag in NVram; shared protobuf telemetry schema.

---

## Q88: Failure Mode Analysis
**Solution:** FMEA: {model corrupt, sensor fail, solver timeout, bad OTA} → mitigations {hash check, redundant sensor, PID fallback, rollback}.

---

## Q89: Scale to 100M Devices
**Solution:** Federated aggregation; edge canary 0.1%; monitor global KPI dashboard; no centralized raw data.

---

## Q90: Interview System Design — "Reduce Video Call Battery 20%"
**Solution:** Profile → radio tail, encoder, display; tactics → adaptive resolution, batch packets, ML-driven frame rate, thermal-aware codec selection; measure with controlled Zoom/FT lab script; ship behind flag.

---

# PART 8: ML THEORY, EVALUATION & BEHAVIORAL (10 Questions)

## Q91: Why MAPE Fails for Power Models
**Solution:** Undefined at zero load; asymmetric penalty. Use MAE in mW + sMAPE; report per workload bucket.

---

## Q92: Train/Val Split for Time Series Logs
**Solution:** No random shuffle — temporal split or split by device session; purged cross-validation for overlapping windows.

---

## Q93: Imbalanced Rare Events (Thermal Shutdown)
**Solution:** Focal loss; oversample shutdown sessions; optimize PR-AUC not accuracy; cost-sensitive learning.

---

## Q94: Uncertainty Quantification
**Solution:** MC dropout or ensemble for prediction interval on T_forecast; widen throttle margin when σ high.

---

## Q95: Explainability for Control Decisions
**Solution:** SHAP on features → "throttled because T_junction high + video workload"; required for firmware engineer trust.

---

## Q96: Offline Policy Evaluation (OPE)
**Solution:** Importance sampling on logged (s,a,r) with behavior policy propensities; doubly robust estimator before shipping RL policy.

---

## Q97: Metric Alignment Across Teams
**Solution:** Single dashboard: {E/session, T_violation_rate, P99_latency, crash_rate}; definitions doc prevents "apples vs oranges."

---

## Q98: Ethics of Battery Throttling
**Solution:** Transparent UX in Low Power Mode; no undisclosed perf limits; user control; internal review for fairness across device ages.

---

## Q99: Behavioral: Ambiguous Scoping
**Problem:** "Make device cooler" — what do you do?

**Solution:** Clarify: skin vs junction? UX impact? Which products? Success metric? Timeline? Propose MVP: reduce video call thermal violations 15% with <5ms latency regression.

---

## Q100: Behavioral: Ship vs Research Tradeoff
**Problem:** Novel RL paper vs reliable PID+MPC for next launch.

**Solution:** Ship MPC with proven bounds; parallelize RL in shadow mode; gated promotion on OPE + canary; align with Apple "simple enough to ship on constrained hardware."

---

# QUICK REFERENCE: Past Apple Patterns → This Role

| Past Question | Energy Tech Adaptation |
|---|---|
| Running median | Q1 thermal median, Q10 P95 latency |
| DocumentStore IR | Q2 power log index |
| Gesture event extraction | Q3 throttling event extraction |
| Parallel countWords | Q4 parallel log processing |
| Encrypted sensor fusion | Q8 encrypted fusion pipeline |
| Priority queue / scheduler | Q5 power budget scheduler |
| Course scheduling / DAG | Q9 power state transitions |
| Matrix multiply | Q12 thermal RC step |
| Design head gestures | Q83 energy-aware gesture system |
| SDE-QA evaluate() | Q3 interval IoU metrics |

---

# STUDY PLAN (2 Weeks)

| Days | Focus | Questions |
|---|---|---|
| 1-2 | Coding heaps, windows, intervals | Q1-Q15 |
| 3-4 | Battery/thermal modeling | Q16-Q30, Q46-Q57 |
| 5-6 | MPC/PID/RL | Q31-Q45 |
| 7-8 | On-device deployment | Q58-Q69 |
| 9-10 | Log analysis + pandas | Q70-Q81 |
| 11-12 | System design mock | Q82-Q90 |
| 13-14 | ML eval + behavioral | Q91-Q100 |

**Mock interview flow (90 min):** 1 coding (Q1/Q3/Q5) + 1 domain (Q31/Q46) + 1 system design (Q82/Q90) + behavioral (Q99/Q100).

Good luck with your Apple Energy Tech interview.
