# Clock Widget

A configurable digital, analogue, and dashboard clock widget that can be embedded in a fence panel.

## Current state

The plugin defines a complete widget capability with 13+ persistent settings across two pages (Display and Behavior). The host can use these settings to render various clock formats in fence panels.

## What it does now

- registers the `widget` capability
- declares two settings pages: **Display** (style, timezone, date, format options) and **Behavior** (refresh intervals, animation, interactions)
- provides stable settings keys for the host to hydrate clock rendering logic
- supports multiple display modes: digital (12/24h), analogue, and dashboard

## Key settings

**Baseline plugin settings:**

- `plugin.show_notifications`: emits notification events to diagnostics when enabled
- `plugin.refresh_interval_seconds`: throttles widget panel refresh operations

**Display page:**

- `style`: digital, analogue, or dashboard mode
- `timezone`: UTC offset or named timezone
- `show_seconds`, `show_date`, `show_day_period`: visibility toggles
- `alignment`, `scale_percent`: layout controls

**Behavior page:**

- `refresh_ms`: update interval
- `blink_separator`: visual emphasis
- `pause_when_hidden`: power optimization
- `sync_to_system_second`: accuracy control
- `smooth_analogue_seconds`: animation smoothness

## What the host still needs

- widget surface rendering (canvas or custom control inside fence panel)
- timezone resolution logic and system time querying
- smooth animation implementation for analogue/dashboard modes
- click-action behaviors (currently defined but unhandled)

## Suggested host direction

Extend the Display page with:

- font family and weight selection
- custom color scheme per widget instance
- local time zone offset fine-tuning

Consider adding templates: "Minimal", "Classic", "Digital Dashboard" as preset configurations.
