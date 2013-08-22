// Clock Example
// Copyright (C) 2006 Litestep Development Team
//
// This program is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License as published by the Free
// Software Foundation; either version 2.1 of the License, or (at your option)
// any later version.
//
// This program is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
// details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA

#if !defined(CLOCK_H)
#define CLOCK_H

#include <windows.h>


class Clock
{
public:
    
    /**
     * Constructs a new module instance. Reads the configuration, registers the
     * window class, creates the window, and registers for Litestep messages.
     *
     * @param hInstance handle to the DLL instance
     */
    Clock(HINSTANCE hInstance);
    
    /**
     * Destructor. Unregisters Litestep messages, destroys the window, and
     * unregisters the window class.
     */
    virtual ~Clock();
    
    /**
     * Returns a handle to this module's window.
     */
    HWND GetWindow() const { return mWindow; }
    
    /**
     * Returns TRUE if the window is always on top or FALSE if it is pinned to
     * the desktop.
     */
    BOOL IsAlwaysOnTop() const { return mAlwaysOnTop; }
    
    /**
     * Returns TRUE if the window is visible.
     */
    BOOL IsVisible() const { return mVisible; }
    
    /**
     * Moves the window relative to its current position.
     *
     * @param x horizontal distance to move in pixels
     * @param y vertical distance to move in pixels
     */
    void MoveBy(int x, int y);
    
    /**
     * Moves the window to a new position.
     *
     * @param x x-coordinate to move to
     * @param y y-coordinate to move to
     */
    void MoveTo(int x, int y);
    
    /**
     * Resizes the window relative to its current size.
     *
     * @param width  number of pixels to add to the width
     * @param height number of pixels to add to the height
     */
    void ResizeBy(int width, int height);
    
    /**
     * Resizes the window to a new size.
     *
     * @param width  new width in pixels
     * @param height new height in pixels
     */
    void ResizeTo(int width, int height);
    
    /**
     * Adjusts the window's z-order so that is either always on top or pinned
     * to the desktop.
     *
     * @param alwaysOnTop TRUE to make the window always on top or FALSE to make
     *                    it pinned to the desktop
     */
    void SetAlwaysOnTop(BOOL alwaysOnTop);
    
    /**
     * Shows or hides the window.
     *
     * @param visible TRUE to show the window or FALSE to hide it
     */
    void SetVisible(BOOL visible);
    
private:
    
    /**
     * Retrieves this module's revision ID string. Litestep displays this string
     * in its about box.
     *
     * @param buffer buffer that receives the revision ID string
     */
    int OnGetRevID(char *buffer);

    /**
     * Updates the window's position when it is moved. Handles WM_MOVE.
     *
     * @param x new x-coordinate
     * @param y new y-coordinate
     */
    void OnMove(int x, int y);
    
    /**
     * Paints the current time. Handles WM_PAINT.
     */
    void OnPaint();
    
    /**
     * Updates the window's size when it is resized. Handles WM_SIZE.
     *
     * @param state  window state. Not used.
     * @param width  new width
     * @param height new height
     */
    void OnSize(UINT state, int width, int height);
    
    /**
     * Force the window to repaint every second. Handles WM_TIMER.
     *
     * @param id timer ID number. Not used.
     */
    void OnTimer(UINT id);
    
    /**
     * Reads this module's configuration data from the step.rc.
     */
    void ReadConfig();
    
    /**
     * Makes the window sticky or nonsticky. A sticky window is not moved when
     * VWM switches between desktops.
     *
     * @param sticky TRUE to make the window sticky or FALSE to make nonsticky
     */
    void SetSticky(BOOL sticky);
    
    /**
     * Dispatches window messages sent to this module's window by calling the
     * appropriate message handler function.
     *
     * @param  message message identifier
     * @param  wParam  message-specific parameter
     * @param  lParam  message-specific parameter
     * @return message-specific return value
     */
    LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
    
    /**
     * Proxies window messages sent to a module window to the module object
     * associated with that window.
     *
     * @param  hWnd    window handle
     * @param  message message identifier
     * @param  wParam  message-specific parameter
     * @param  lParam  message-specific parameter
     * @return message-specific return value
     */
    static LRESULT CALLBACK WindowProcProxy(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    
private:
    
    HINSTANCE mInstance; ///< Handle to DLL instance
    HWND mWindow;        ///< Handle to window
    
    BOOL mAlwaysOnTop;   ///< Is window always on top or pinned to desktop?
    BOOL mVisible;       ///< Is window visible?
    int mX;              ///< X-coordinate of window on screen
    int mY;              ///< Y-coordinate of window on screen
    int mWidth;          ///< Window's width
    int mHeight;         ///< Window's height
};


#endif // CLOCK_H
