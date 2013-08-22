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

#if !defined(LABEL_H)
#define LABEL_H

#include "common.h"

class Label
{
public:

    /**
     * Constructs a new label.
     */
    Label();
    
    /**
     * Destructor.
     */
    virtual ~Label();
    
    /**
     * Creates this label's window.
     *
     * @param instance  handle to owning module
     * @return          <code>true</code> if window created successfully;
     *                  <code>false</code> otherwise
     */
    bool Create(HINSTANCE instance);
    
    /**
     * Destroys this label's window.
     */
    void Destroy();
    
    /**
     * Registers the label window class.
     *
     * @param instance  handle to owning module
     */
    static void RegisterWindowClass(HINSTANCE instance);
    
    /**
     * Unregisters the label window class.
     *
     * @param instance  handle to owning module
     */
    static void UnregisterWindowClass(HINSTANCE instance);
    
private:

    /**
     * Draws this label's background and borders.
     *
     * @param dc  handle to device context in which to draw
     */
    void DrawBackground(HDC dc);
    
    /**
     * Draws this label's image.
     *
     * @param dc  handle to device context in which to draw
     * @param x   x-coordinate at which to draw the image
     * @param y   y-coordinate at which to draw the image
     */
    void DrawImage(HDC dc, int x, int y);
    
    /**
     * Draws this label's text.
     *
     * @param dc  handle to device context in which to draw
     * @param x   x-coordinate at which to draw the text
     * @param y   y-coordinate at which to draw the text
     */
    void DrawText(HDC dc, int x, int y);
    
    /**
     * Gets the size of this label's text when drawn using this label's font.
     *
     * @param dc      handle to device context that defines font metrics
     * @param width   pointer to an integer that receives the text's width
     * @param height  pointer to an integer that receives the text's height
     */
    void GetTextExtent(HDC dc, int *width, int *height);
    
    /**
     * Paints this label into the specified device context.
     *
     * @param dc  handle to device context into which to paint
     */
    void Paint(HDC dc);
    
    /**
     * Forces a repaint of this label.
     */
    void Repaint();
    
    /**
     * Changes this label's z-order so that it is either always on top or pinned
     * to the desktop.
     *
     * @param alwaysOnTop  If <code>true</code>, puts this label always on top.
     *                     If <code>false</code>, pins this label to the
     *                     desktop.
     */
    void SetAlwaysOnTop(bool alwaysOnTop);
    
    /**
     * Makes this label sticky or nonsticky. A sticky window is not moved when
     * the VWM switches desktops.
     *
     * @param sticky  If <code>true</code>, makes this label sticky. If
     *                <code>false</code>, makes this label not sticky.
     */
    void SetSticky(bool sticky);
    
    /**
     * Shows or hides this label.
     *
     * @param visible  If <code>true</code>, shows this label. If
     *                 <code>false</code>, hides this label.
     */
    void SetVisible(bool visible);
    
private:

    /**
     * Gets this module's version string for display in Litestep's about box.
     *
     * @param buffer  string buffer to copy the version string into
     * @return        length of the version string
     */
    int OnGetRevID(LPTSTR buffer);
    
    /**
     * Updates this label's position in response to <code>WM_MOVE</code>.
     *
     * @param x  x-coordinate that the label has been moved to
     * @param y  y-coordinate that the label has been moved to
     */
    void OnMove(int x, int y);
    
    /**
     * Paints this label in response to <code>WM_PAINT</code>.
     */
    void OnPaint();
    
    /**
     * Updates this label's size in response to <code>WM_SIZE</code>.
     *
     * @param state   type of sizing operation performed on the label
     * @param width   width that the label has been resized to
     * @param height  height that the label has been resized to
     */
    void OnSize(UINT state, int width, int height);
    
    /**
     * Handles window messages sent to this label.
     *
     * @param message  message identifier
     * @param wParam   message-specific parameter
     * @param lParam   message-specific parameter
     * @return         message-specific return value
     */
    LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
    
    /**
     * Proxies window messages sent a label window to the object associated with
     * that window.
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
     * If <code>true</code>, this label is always on top. If <code>false</code>,
     * this label is pinned to the desktop.
     */
    bool mAlwaysOnTop;
    
    /**
     * If <code>true</code>, this label is visible; <code>false</code>
     * otherwise.
     */
    bool mVisible;
    
    /**
     * The handle to this label's window.
     */
    HWND mWindow;
    
    /**
     * The x-coordinate of this label relative to the desktop.
     */
    int mX;
    
    /**
     * The y-coordinate of this label relative to the desktop.
     */
    int mY;
    
    /**
     * The width of this label in pixels.
     */
    int mWidth;
    
    /**
     * The height of this label in pixels.
     */
    int mHeight;
};

#endif // LABEL_H
