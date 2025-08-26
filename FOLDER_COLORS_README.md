# Folder Colors Setup Guide

## To get folder colors working in VS Code:

### 1. Install Required Extension
Install the **Folder Colorizer** extension:
- Extension ID: `alefragnani.folder-colorizer`
- Or search for "Folder Colorizer" in VS Code extensions

### 2. Reload VS Code
After installing the extension, reload VS Code (Command Palette → "Developer: Reload Window")

### 3. Open as Workspace
Open the project using the workspace file: `Transfer Arm_TA June25.code-workspace`

### 4. Color Scheme
The folders are configured with the following color scheme:

- 🏠 **Main (Blue)**: `src/`, `include/` - Core application files
- ⚙️ **Functionality/Config (Green)**: Configuration files and pins definitions
- 🔄 **Motors/StateMachine (Purple/Yellow)**: Movement and state logic
- 🎨 **Painting Sides (Cyan)**: Painting-specific code in PICK_CYCLE
- 🔌 **Hardware (Orange)**: Hardware interfaces in `lib/`
- 💾 **Storage (Purple)**: Data persistence in `data/`
- 🧪 **Testing (Purple)**: Test files in `test/`

### 5. Troubleshooting
If colors still don't appear:
- Check that the Folder Colorizer extension is enabled
- Try switching to the workspace file view
- Restart VS Code completely
- Check the VS Code output panel for any error messages

### 6. Manual Configuration
If automatic configuration doesn't work, you can manually set folder colors:
1. Right-click on a folder in the explorer
2. Select "Folder Color"
3. Choose a color from the palette
4. The color will be saved in `.vscode/settings.json`
