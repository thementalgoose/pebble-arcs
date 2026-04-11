#!/usr/bin/env python3
"""
Build the Pebble app, then run a sequence of emulator steps.

Usage:
    python screenshot.py --emulator basalt --steps "wait:2000,screenshot:home.png"
    python screenshot.py --emulator chalk --steps "button:select,wait:500,screenshot:screens/detail.png"
    python screenshot.py --emulator basalt --steps steps.json

Step format (comma-separated string or JSON file):
    wait:<ms>                              - sleep for <ms> milliseconds
    button:<btn>                           - click a button (back/up/select/down)
    button:<btn>:<duration_ms>             - long-press a button for <duration_ms> ms
    button:<btn>:repeat:<n>                - click a button <n> times
    button:<btn>:repeat:<n>:<interval_ms>  - click <n> times with interval
    screenshot:<filename>                  - take a screenshot and save to <filename>
"""

import argparse
import json
import subprocess
import sys
import time
from pathlib import Path

VALID_BUTTONS = {"back", "up", "select", "down"}
VALID_EMULATORS = {"aplite", "basalt", "chalk", "diorite", "emery", "flint", "gabbro"}
PROJECT_DIR = Path(__file__).parent


def run(cmd, check=True):
    print(f"  $ {' '.join(cmd)}")
    result = subprocess.run(cmd, cwd=PROJECT_DIR)
    if check and result.returncode != 0:
        print(f"Command failed with exit code {result.returncode}", file=sys.stderr)
        sys.exit(result.returncode)
    return result


def parse_steps(steps_arg):
    """Parse steps from a comma-separated string or a JSON file path."""
    if steps_arg is None:
        return []

    path = Path(steps_arg)
    if path.exists() and path.suffix == ".json":
        with open(path) as f:
            return json.load(f)

    # Comma-separated inline steps — split on commas but preserve path separators
    return [s.strip() for s in steps_arg.split(",") if s.strip()]


def execute_step(step, emulator, vnc, scale, folder):
    """Execute a single step string against the running emulator."""
    # Split on ':' but only the first token determines the type; paths may contain /
    type_end = step.index(":") if ":" in step else len(step)
    step_type = step[:type_end]
    rest = step[type_end + 1:] if type_end < len(step) else ""

    if step_type == "wait":
        ms = int(rest)
        print(f"  [wait] {ms}ms")
        time.sleep(ms / 1000.0)

    elif step_type == "screenshot":
        filename = rest
        if not filename:
            raise ValueError("screenshot step requires a filename: 'screenshot:<filename>'")
        if folder:
            filename = str(Path(folder) / filename)
        # Ensure parent directory exists
        Path(filename).parent.mkdir(parents=True, exist_ok=True)
        print(f"  [screenshot] -> {filename}")
        cmd = ["pebble", "screenshot", "--emulator", emulator, "--no-open"]
        if vnc:
            cmd.append("--vnc")
        if scale is not None:
            cmd += ["--scale", str(scale)]
        cmd.append(filename)
        run(cmd)

    elif step_type == "button":
        parts = rest.split(":")
        btn = parts[0]
        if btn not in VALID_BUTTONS:
            raise ValueError(f"Invalid button '{btn}'. Valid: {', '.join(sorted(VALID_BUTTONS))}")

        cmd = ["pebble", "emu-button", "--emulator", emulator]
        if vnc:
            cmd.append("--vnc")

        # button:<btn>                       -> click
        # button:<btn>:<duration_ms>         -> click --duration <ms>
        # button:<btn>:repeat:<n>            -> click --repeat <n>
        # button:<btn>:repeat:<n>:<interval> -> click --repeat <n> --interval <ms>
        if len(parts) == 1:
            cmd += ["click", btn]
            print(f"  [button] click {btn}")
        elif parts[1] == "repeat":
            repeat = int(parts[2]) if len(parts) > 2 else 1
            interval = int(parts[3]) if len(parts) > 3 else None
            cmd += ["click", btn, "--repeat", str(repeat)]
            if interval is not None:
                cmd += ["--interval", str(interval)]
            print(f"  [button] click {btn} x{repeat}" + (f" every {interval}ms" if interval else ""))
        else:
            duration = int(parts[1])
            cmd += ["click", btn, "--duration", str(duration)]
            print(f"  [button] click {btn} ({duration}ms hold)")

        run(cmd)

    else:
        raise ValueError(f"Unknown step type '{step_type}'. Valid types: wait, button, screenshot")


def main():
    parser = argparse.ArgumentParser(
        description="Build the Pebble app and run a sequence of emulator steps."
    )
    parser.add_argument(
        "--emulator", "-e",
        required=True,
        choices=sorted(VALID_EMULATORS),
        help="Emulator platform to target",
    )
    parser.add_argument(
        "--steps", "-s",
        default=None,
        help=(
            "Steps to run after installing. Either a comma-separated string "
            "(e.g. 'wait:1000,button:select,screenshot:out.png') "
            "or a path to a JSON file containing a list of step strings."
        ),
    )
    parser.add_argument(
        "--folder", "-f",
        default=None,
        help="Directory prefix applied to all screenshot step filenames",
    )
    parser.add_argument(
        "--no-build",
        action="store_true",
        help="Skip the build step (use existing build artifacts)",
    )
    parser.add_argument(
        "--scale",
        type=int,
        default=None,
        help="Screenshot scale factor passed to all screenshot steps (e.g. --scale 6)",
    )
    parser.add_argument(
        "--vnc",
        action="store_true",
        help="Pass --vnc to all emulator commands (required in headless environments)",
    )
    args = parser.parse_args()

    steps = parse_steps(args.steps)

    # --- Build ---
    if not args.no_build:
        print("\n[1/2] Building...")
        run(["pebble", "build"])
    else:
        print("\n[1/2] Skipping build.")

    # --- Install ---
    print(f"\n[2/2] Installing on {args.emulator} emulator...")
    install_cmd = ["pebble", "install", "--emulator", args.emulator]
    if args.vnc:
        install_cmd.append("--vnc")
    run(install_cmd)

    # --- Steps ---
    if steps:
        print(f"\nRunning {len(steps)} step(s)...")
        for i, step in enumerate(steps, 1):
            print(f"  Step {i}/{len(steps)}: {step}")
            execute_step(step, args.emulator, args.vnc, args.scale, args.folder)
    else:
        print("\nNo steps defined.")

    # --- Kill emulator ---
    print("\nStopping emulator...")
    run(["pebble", "kill"])

    print("\nDone.")


if __name__ == "__main__":
    main()
