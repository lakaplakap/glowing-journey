# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Overview

面向对象图表生成系统 — a Qt Widgets application (Qt6, qmake) demonstrating polymorphic chart rendering. Three chart types (histogram, line, pie) share a common abstract base class and render from a shared `DataSource`.

## Build

Qt 6.11.1 is installed at `D:\Qt\6.11.1\mingw_64\`, with MinGW 13.1.0 toolchain at `D:\Qt\Tools\mingw1310_64\`.

```bash
# From the source directory, regenerate Makefile then build:
cd chart-project
export PATH="/d/Qt/Tools/mingw1310_64/bin:/d/Qt/6.11.1/mingw_64/bin:$PATH"
/d/Qt/6.11.1/mingw_64/bin/qmake.exe chart-project.pro -spec win32-g++ "CONFIG+=debug"
/d/Qt/Tools/mingw1310_64/bin/mingw32-make.exe

# Or from the existing build directory:
cd chart-project/build/Desktop_x86_windows_msys_pe_64bit-Debug
export PATH="/d/Qt/Tools/mingw1310_64/bin:/d/Qt/6.11.1/mingw_64/bin:$PATH"
/d/Qt/Tools/mingw1310_64/bin/mingw32-make.exe

# Clean rebuild:
/d/Qt/Tools/mingw1310_64/bin/mingw32-make.exe clean && /d/Qt/Tools/mingw1310_64/bin/mingw32-make.exe
```

The project also supports Qt5 via conditional compilation for `SkipEmptyParts` (see `chart.cpp:92-96`).

## Architecture

```
DataSource (QObject)          ← owns data, emits dataChanged()
    │
    │  raw pointer, injected via setDataSource()
    ▼
Chart (abstract QWidget)      ← Template Method: paintEvent() → virtual draw()
    │
    ├── Histogram             ← bar chart with rounded-rect bars
    ├── LineChart             ← line chart with point markers + area fill
    └── PieChart              ← pie chart with HSL slices, legend, center decoration
```

**Key patterns:**
- **Template Method**: `Chart::paintEvent()` handles QPainter setup (antialiasing) and delegates to `draw()`. Subclasses only implement `draw()`.
- **Observer via Qt signals**: `DataSource::dataChanged` → `Chart::update()` — any data change auto-refreshes all connected charts, plus the data table and status bar in main.cpp.
- **Polymorphic switching**: `main.cpp` holds a `Chart*` pointer; chart-switch buttons `deleteLater()` the old widget and `new` the desired subtype.
- **`setDataSource` manages signal wiring**: disconnects the old source, connects the new one, calls `update()` immediately.

## Data flow

1. `DataSource` stores data in `QVector<double> m_data` (no fixed size limit).
2. `DataSource::loadFromFile()` reads the entire file and uses `QRegularExpression("[,\\s]+")` to split — any plain-text file with comma/space/newline-delimited numbers works, not just .txt/.csv/.dat.
3. `DataSource::generateRandom(count, min, max)` fills data with random values rounded to 1 decimal place.
4. `DataSource::setData()` accepts a `QVector<double>` directly.
5. All three mutators emit `dataChanged()` → all connected charts call `update()` → Qt triggers `paintEvent()` → `draw()` re-executes.

## GUI layout (main.cpp)

- **Top toolbar** (`QGroupBox "图表操作"`): chart-type buttons (histogram/line/pie) + data-operation buttons (load file / random generate / clear)
- **Splitter**: chart area (75%) / data table (25%) — `QSplitter(Qt::Vertical)`
- **StatusBar**: count, sum, min, max — refreshed on every `dataChanged` signal
- **Button highlight**: uses dynamic `"active"` property + `unpolish()`/`polish()` to force stylesheet re-evaluation

## File map

| File | Purpose |
|------|---------|
| `chart-project.pro` | qmake project (Qt6, C++17, widgets module) |
| `chart.h` | `DataSource` + `Chart`/`Histogram`/`LineChart`/`PieChart` declarations |
| `chart.cpp` | All method implementations — data parsing, random generation, paint logic |
| `main.cpp` | Window setup, button wiring, chart switching, stylesheet, data table, status bar |

## Qt5/Qt6 compatibility

`chart.cpp:92-96` uses `QT_VERSION` preprocessor check for `QString::split()` second argument:
- Qt6: `Qt::SkipEmptyParts`
- Qt5: `QString::SkipEmptyParts`
