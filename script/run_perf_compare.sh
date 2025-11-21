#!/usr/bin/env bash
# 一键对照测试脚本
# 构建两个对照组版本，build-baseline 禁用 likely 分支预测，build-modified 启用
# 再运行一些 perf_test 的用例
# 需要其他脚本或人工分析日志结果

set -euo pipefail
ROOT_DIR=$(cd "$(dirname "$0")/.." && pwd)
BUILD_BASE=${ROOT_DIR}/build-baseline
BUILD_MOD=${ROOT_DIR}/build-modified
REPEATS=${REPEATS:-6}
WARMUP=${WARMUP:-1}
CASES=(${@:-micro})
RUNTIME_MS=${RUNTIME_MS:-200}
OVERHEAD=${OVERHEAD:-5}
LOG=${ROOT_DIR}/script/perf_compare.log

echo "run at: $(date)" > "$LOG"

# Build baseline (yyjson_likely/unlikely -> nop)
echo "Configuring baseline in $BUILD_BASE" | tee -a "$LOG"
# Escape parentheses when defining function-like macros in CMake flags
cmake -S "$ROOT_DIR" -B "$BUILD_BASE" -DCMAKE_BUILD_TYPE=Release \
  -DBUILD_PERF=ON \
  -DCMAKE_CXX_FLAGS="-Dyyjson_likely\(x\)=\(x\) -Dyyjson_unlikely\(x\)=\(x\)" | tee -a "$LOG"
cmake --build "$BUILD_BASE" -j 2 | tee -a "$LOG"

# Build modified
echo "Configuring modified in $BUILD_MOD" | tee -a "$LOG"
cmake -S "$ROOT_DIR" -B "$BUILD_MOD" -DCMAKE_BUILD_TYPE=Release -DBUILD_PERF=ON | tee -a "$LOG"
cmake --build "$BUILD_MOD" -j 2 | tee -a "$LOG"

run_case(){
  local BUILD_DIR="$1"
  local CASE="$2"
  local BIN="$BUILD_DIR/perf_test"
  if [ ! -x "$BIN" ]; then
    echo "perf_test not found at $BIN" | tee -a "$LOG"
    return 1
  fi
  echo "--- $BUILD_DIR $CASE ---" | tee -a "$LOG"
  for i in $(seq 1 $WARMUP); do
    echo "warmup $i" | tee -a "$LOG"
  # allow warmup failures to not abort the whole script
  taskset -c 0 "$BIN" "$CASE" --runtime_ms=$RUNTIME_MS --overhead_percent=$OVERHEAD 2>&1 | tee -a "$LOG" || true
  done
  for i in $(seq 1 $REPEATS); do
    echo "run $i" | tee -a "$LOG"
    taskset -c 0 "$BIN" "$CASE" --runtime_ms=$RUNTIME_MS --overhead_percent=$OVERHEAD 2>&1 | tee -a "$LOG" || true
  done
}

for CASE in "${CASES[@]}"; do
  # tolerate failures in run_case so one failing test doesn't stop the whole run
  run_case "$BUILD_BASE" "$CASE" || true
  run_case "$BUILD_MOD" "$CASE" || true
done

echo "done at: $(date)" | tee -a "$LOG"

echo "logs: $LOG"
