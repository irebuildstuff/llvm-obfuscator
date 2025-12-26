# VirusTotal UI Design Reference

## Overview
This document captures the design patterns, color scheme, and UI components extracted from VirusTotal.com for use in the LLVM Obfuscator web application.

## Color Palette

### Primary Colors
- **Background**: `#161625` (rgb(22, 22, 37)) - Dark blue-gray
- **Text**: `#ffffff` (rgb(255, 255, 255)) - White
- **Primary Accent**: `#86aaf9` (rgb(134, 170, 249)) - Light blue
- **Primary RGB**: `134, 170, 249`

### Status Colors
- **Success**: `#54ab98` (rgb(84, 171, 152)) - Teal
- **Warning**: `#ffed2e` (rgb(255, 237, 46)) - Yellow
- **Danger/Critical**: `#ff5a50` (rgb(255, 90, 80)) - Red
- **Info**: `#42c3d0` (rgb(66, 195, 208)) - Cyan
- **Malachite**: `#01d667` (rgb(1, 214, 103)) - Green

### Secondary Colors
- **Secondary Background**: `#232c42` (rgb(35, 44, 66))
- **Tertiary Background**: `#2f3d5c` (rgb(47, 61, 92))
- **Border Color**: `#3d4a69` (rgb(61, 74, 105))
- **Input Border**: `#6275a3` (rgb(98, 117, 163))

### Gray Scale
- **Gray 100**: `#f9f9f9`
- **Gray 200**: `#f2f2f2`
- **Gray 300**: `#e6e6e6`
- **Gray 400**: `#ccc`
- **Gray 500**: `#b3b3b3`
- **Gray 600**: `#666`
- **Gray 700**: `#4d4d4d`
- **Gray 800**: `#333`
- **Gray 900**: `#1a1a1a`

## Typography

### Font Family
- **Primary**: `"Source Sans Pro", Roboto, RobotoDraft, Helvetica, Arial, sans-serif`
- **Monospace**: `"Source Code Pro", monospace`

### Font Sizes
- **Body**: `1rem` (14px base)
- **Default**: `14px`
- **Item Report**: `1rem`
- **Detections List**: `1rem`
- **Expandable Entry**: `1rem`

### Font Weights
- **Body**: `400` (normal)
- **Line Height**: `1.5`

## Layout

### Max Width
- **Max Page Width**: `1300px`

### Border Radius
- **Small**: `0.25rem`
- **Default**: `0.375rem`
- **Large**: `0.5rem`
- **XL**: `1rem`
- **2XL**: `2rem`
- **XXL**: `2rem`
- **Pill**: `50rem`

### Spacing
- **Dialog Padding**: `24px`
- **Border Width**: `1px`

## Component Patterns

### File Upload Area
- Large, prominent upload zone
- Drag-and-drop interface
- "Choose file" button
- Visual feedback on hover/drag
- Terms and privacy notice below upload area

### Navigation
- Top banner with logo
- Search bar in header
- Upload file button
- Notifications icon
- Sign in/Sign up links

### Tabs
- Tab list for different input types (File, URL, Search)
- Active tab highlighted
- Clean, minimal design

### Buttons
- Primary buttons use accent color (`#86aaf9`)
- Hover states with lighter shades
- Icon buttons with consistent sizing

### Cards/Sections
- Dark background with subtle borders
- Rounded corners
- Clear section separation

## CSS Variables Reference

Key CSS variables extracted from VirusTotal:
- `--bs-primary`: `#86aaf9`
- `--bs-body-bg`: `#161625`
- `--bs-body-color`: `#fff`
- `--bs-border-color`: `#3d4a69`
- `--bs-success`: `#54ab98`
- `--bs-danger`: `#ff5a50`
- `--bs-warning`: `#ffed2e`
- `--bs-info`: `#42c3d0`
- `--bs-link-hover-color`: `#86aaf9`
- `--max-page-width`: `1300px`

## UI Components to Replicate

1. **Header/Navigation**
   - Logo placement
   - Search bar
   - Action buttons (Upload, Notifications)
   - User account links

2. **File Upload Interface**
   - Large drag-and-drop area
   - File selection button
   - Visual feedback states
   - Terms acceptance text

3. **Results Display**
   - Tabbed interface for different sections
   - Status indicators with color coding
   - Metrics display cards
   - Expandable/collapsible sections

4. **Report Visualization**
   - Key-value tables
   - Statistics charts
   - Color-coded status badges
   - Progress indicators

## Screenshots
- `virustotal-homepage.png` - Full page screenshot of homepage with file upload

## Notes
- Dark theme throughout
- High contrast for readability
- Consistent spacing and alignment
- Professional, security-focused aesthetic
- Clean, minimal design with focus on functionality


