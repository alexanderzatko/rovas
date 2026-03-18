# Wallet Fullness Gauge Module

## Overview

This Drupal 7 module provides a field formatter that creates a visual gauge to show the current level of a wallet compared to its threshold value. The gauge updates dynamically when the page reloads and allows users to click on it to see the exact numeric values.

## Features

- Visual representation of wallet fullness as a horizontal gauge
- Multiple color schemes to choose from
- Click to reveal exact numeric values
- Customizable formatter settings
- Works with any number field types (integer, decimal, float)

## Installation

1. Download the module and place it in your `sites/all/modules/custom` directory
2. Enable the module through the Drupal admin interface or using drush:
   ```
   drush en wallet_fullness -y
   ```

## Usage

### Basic Configuration

1. Go to the "Manage Display" settings for the content type where you want to use the gauge
2. For the field that contains the current wallet value, change the format to "Wallet Fullness Gauge"
3. Click the gear icon to configure the formatter settings
4. Select the field that contains the threshold value
5. Choose whether to display numeric values on click
6. Select a color scheme
7. Save your settings

### Example Use Case

For example, to display the gauge for "PA wallet current level" with "Preferred allocation threshold" as the threshold:

1. Go to the "Manage Display" settings for your content type
2. For the "PA wallet current level" field, change the format to "Wallet Fullness Gauge"
3. In the settings, select "Preferred allocation threshold" as the threshold field
4. Choose your desired color scheme and click settings
5. Save the display settings

## Color Schemes

- **Default (Blue)**: A blue gradient, suitable for neutral displays
- **Green**: A green gradient, suggesting positive progress
- **Red-Yellow-Green**: Changes color based on fullness percentage:
  - 0-33%: Red (low)
  - 34-66%: Yellow (medium)
  - 67-100%: Green (high)

## Customization

You can customize the appearance of the gauge by modifying the CSS in `wallet_fullness.css`.

## Troubleshooting

- If the gauge doesn't appear, ensure both fields (current value and threshold) have valid numeric values
- If the styling looks incorrect, try clearing the Drupal cache
- For any other issues, check the Drupal error logs

## Requirements

- Drupal 7.x
- Field UI module enabled
