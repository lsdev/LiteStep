// Tutorial
// Copyright (C) 2006 Litestep Development Team
//
// This program is free software; you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free Software
// Foundation; either version 2 of the License, or (at your option) any later
// version.
//
// This program is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along with
// this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

#if !defined(LABELMANAGER_H)
#define LABELMANAGER_H

#include "common.h"
#include "Label.h"

// Maximum number of labels
#define MAX_LABELS 100

class LabelManager
{
public:

    /**
     * Constructors a new label manager.
     */
    LabelManager();
    
    /**
     * Destructor.
     */
    virtual ~LabelManager();
    
    /**
     * Creates this label manager's window (which handles Litestep messages) and
     * creates the labels specified in the Litestep configuration file.
     *
     * @param instance  handle to owning module
     * @return          <code>true</code> if all operations succeeded;
     *                  <code>false</code> if something failed
     */
    bool Create(HINSTANCE instance);
    
    /**
     * Creates a new label.
     *
     * @param name  a name that uniquely identifies the label
     */
    void CreateLabel(LPCTSTR name);
    
    /**
     * Destroys this label manager's window and all the labels.
     */
    void Destroy();
    
    /**
     * Destroys a label.
     *
     * @param name  name of the label to destroy
     */
    void DestroyLabel(LPCTSTR name);
    
    /**
     * Finds a label by name.
     *
     * @param name  name of the label to find
     * @return      a pointer to the label if found; <code>NULL</code> otherwise
     */
    Label *FindLabel(LPCTSTR name);
    
    /**
     * Registers the label manager window class.
     *
     * @param instance  handle to owning module
     */
    static void RegisterWindowClass(HINSTANCE instance);
    
    /**
     * Unregisters the label manager window class.
     *
     * @param instance  handle to owning module
     */
    static void UnregisterWindowClass(HINSTANCE instance);
    
private:

    /**
     * Creates the labels specified in the Litestep configuration file.
     */
    void CreateLabels();
    
private:

    /**
     * Gets this module's version string for display in Litestep's about box.
     *
     * @param buffer  string buffer to copy the version string into
     * @return        length of the version string
     */
    int OnGetRevID(LPTSTR buffer);
    
    /**
     * Handles window messages sent to this label manager.
     *
     * @param message  message identifier
     * @param wParam   message-specific parameter
     * @param lParam   message-specific parameter
     * @return         message-specific return value
     */
    LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
    
    /**
     * Proxies window messages sent a label manager window to the object
     * associated with that window.
     *
     * @param window   handle to the window
     * @param message  message identifier
     * @param wParam   message-specific parameter
     * @param lParam   message-specific parameter
     * @return         message-specific return value
     */
    static LRESULT WINAPI WindowProcProxy(HWND window, UINT message, WPARAM wParam, LPARAM lParam);
    
private:

    /**
     * Handle to the module that owns this label manager.
     */
    HINSTANCE mInstance;
    
    /**
     * An array of pointers to labels managed by this label manager.
     */
    Label *mLabels[MAX_LABELS];
    
    /**
     * Number of labels in the <code>mLabels</code> array.
     */
    int mNumLabels;
    
    /**
     * Handle to this label manager's window.
     */
    HWND mWindow;
};


#endif // LABELMANAGER_H
