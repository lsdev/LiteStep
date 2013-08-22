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
     * @param name      string containing a name for this label
     * @param instance  handle to owning module
     * @return          <code>true</code> if window created successfully;
     *                  <code>false</code> otherwise
     */
    bool Create(LPCTSTR name, HINSTANCE instance);
    
    /**
     * Destroys this label's window.
     */
    void Destroy();
    
    /**
     * Returns this label's name.
     *
     * @return  this label's name
     */
    LPCTSTR GetName() const { return mName; }
    
    /**
     * Returns <code>true</code> if this label is always on top;
     * <code>false</code> if this label is pinned to the desktop.
     *
     * @return  <code>true</code> if this label is always on top;
     *          <code>false</code> if this label is pinned to the desktop
     */
    bool IsAlwaysOnTop() const { return mAlwaysOnTop; }
    
    /**
     * Returns <code>true</code> if this label is visible; <code>false</code>
     * otherwise.
     *
     * @return  <code>true</code> if this label is visible; <code>false</code>
     *          otherwise
     */
    bool IsVisible() const { return mVisible; }
    
    /**
     * Moves this label relative to its current position.
     *
     * @param x  distance to move horizontally
     * @param y  distance to move vertically
     */
    void MoveBy(int x, int y);
    
    /**
     * Moves this label to a new position.
     *
     * @param x  x-coordinate to move this label to
     * @param y  y-coordinate to move this label to
     */
    void MoveTo(int x, int y);
    
    /**
     * Registers the label window class.
     *
     * @param instance  handle to owning module
     */
    static void RegisterWindowClass(HINSTANCE instance);
    
    /**
     * Resizes this label relative to its current size.
     *
     * @param width   amount to resize horizontally
     * @param height  amount to resize vertically
     */
    void ResizeBy(int width, int height);
    
    /**
     * Resizes this label.
     *
     * @param width   width to resize to
     * @param height  height to resize to
     */
    void ResizeTo(int width, int height);
    
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
     * Sets the image displayed by this label.
     *
     * @param fileName  path to an image file to be displayed
     */
    void SetImage(LPCTSTR fileName);
    
    /**
     * Sets the text displayed by this label.
     *
     * @param text  string containing text to be displayed
     */
    void SetText(LPCTSTR text);
    
    /**
     * Shows or hides this label.
     *
     * @param visible  If <code>true</code>, shows this label. If
     *                 <code>false</code>, hides this label.
     */
    void SetVisible(bool visible);
    
    /**
     * Unregisters the label window class.
     *
     * @param instance  handle to owning module
     */
    static void UnregisterWindowClass(HINSTANCE instance);
    
private:

    /**
     * Creates a region containing the non-transparent parts of this label's
     * background.
     *
     * @return  handle to a region containing the non-transparent parts of this
     *          label's background
     */
    HRGN CreateBackgroundRegion();
    
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
     * Read this label's configuration from Litestep's configuration file.
     */
    void ReadConfig();
    
    /**
     * Forces a repaint of this label.
     */
    void Repaint();
    
    /**
     * Makes this label sticky or nonsticky. A sticky window is not moved when
     * the VWM switches desktops.
     *
     * @param sticky  If <code>true</code>, makes this label sticky. If
     *                <code>false</code>, makes this label not sticky.
     */
    void SetSticky(bool sticky);
    
private:

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
     * The horizontal alignment of this label's image and text relative to its
     * window. One of the following values:
     * <ul>
     * <li><code>ALIGN_LEFT</code></li>
     * <li><code>ALIGN_CENTER</code></li>
     * <li><code>ALIGN_RIGHT</code></li>
     * </ul>
     */
    int mAlign;
    
    /**
     * If <code>true</code>, this label is always on top. If <code>false</code>,
     * this label is pinned to the desktop.
     */
    bool mAlwaysOnTop;
    
    /**
     * The color used to paint this label's background if there is no background
     * image.
     */
    COLORREF mBackgroundColor;
    
    /**
     * The handle to the image used to paint this label's background. If
     * <code>NULL</code>, then this label's background is painted with the color
     * specified by <code>mBackgroundColor</code> instead.
     */
    HBITMAP mBackgroundImage;
    
    /**
     * The number of pixels along the top edge of this label's background
     * image that will not be stretched or tiled.
     */
    int mBackgroundImageBorderTop;
    
    /**
     * The number of pixels along the right edge of this label's background
     * image that will not be stretched or tiled.
     */
    int mBackgroundImageBorderRight;
    
    /**
     * The number of pixels along the bottom edge of this label's background
     * image that will not be stretched or tiled.
     */
    int mBackgroundImageBorderBottom;
    
    /**
     * The number of pixels along the left edge of this label's background
     * image that will not be stretched or tiled.
     */
    int mBackgroundImageBorderLeft;
    
    /**
     * The method used to stretch and/or tile this label's background image in
     * order to fill this label's window. One of the following values:
     * <ul>
     * <li><code>SCALE_STRETCH</code></li>
     * <li><code>SCALE_HORIZONTALTILE</code></li>
     * <li><code>SCALE_VERTICALTILE</code></li>
     * <li><code>SCALE_TILE</code></li>
     * </ul>
     */
    int mBackgroundImageTile;
    
    /**
     * The color used to paint this label's top border.
     */
    COLORREF mBorderColorTop;
    
    /**
     * The color used to paint this label's right border.
     */
    COLORREF mBorderColorRight;
    
    /**
     * The color used to paint this label's bottom border.
     */
    COLORREF mBorderColorBottom;
    
    /**
     * The color used to paint this label's left border.
     */
    COLORREF mBorderColorLeft;
    
    /**
     * The width of this label's top border in pixels.
     */
    int mBorderTop;
    
    /**
     * The width of this label's right border in pixels.
     */
    int mBorderRight;
    
    /**
     * The width of this label's bottom border in pixels.
     */
    int mBorderBottom;
    
    /**
     * The width of this label's left border in pixels.
     */
    int mBorderLeft;
    
    /**
     * The handle to the font used to paint this label's text.
     */
    HFONT mFont;
    
    /**
     * The color used to paint this label's text.
     */
    COLORREF mFontColor;
    
    /**
     * If <code>true</code>, this label's text is painted with a shadow.
     */
    bool mFontShadow;
    
    /**
     * The color used to paint this label's text shadow.
     */
    COLORREF mFontShadowColor;
    
    /**
     * The number of pixels that this label's text shadow is offset from its
     * text horizontally.
     */
    int mFontShadowOffsetX;
    
    /**
     * The number of pixels that this label's text shadow is offset from its
     * text vertically.
     */
    int mFontShadowOffsetY;
    
    /**
     * The handle to the image displayed by this label. If <code>NULL</code>,
     * this label does not display an image.
     */
    HBITMAP mImage;
    
    /**
     * The position of this label's image relative to its text. One of the
     * following values:
     * <ul>
     * <li><code>IMAGE_POSITION_LEFT</code></li>
     * <li><code>IMAGE_POSITION_TOP</code></li>
     * <li><code>IMAGE_POSITION_RIGHT</code></li>
     * <li><code>IMAGE_POSITION_BOTTOM</code></li>
     * </ul>
     */
    int mImagePosition;
    
    /**
     * The number of pixels between this label's image and its text.
     */
    int mImageTextGap;
    
    /**
     * The name used to uniquely identify this label.
     */
    TCHAR mName[MAX_NAME];
    
    /**
     * The number of pixels between this label's top edge and its contents.
     */
    int mPaddingTop;
    
    /**
     * The number of pixels between this label's right edge and its contents.
     */
    int mPaddingRight;
    
    /**
     * The number of pixels between this label's bottom edge and its contents.
     */
    int mPaddingBottom;
    
    /**
     * The number of pixels between this label's left edge and its contents.
     */
    int mPaddingLeft;
    
    /**
     * The text displayed by this label.
     */
    TCHAR mText[MAX_TEXT];
    
    /**
     * The vertical alignment of this label's image and text relative to its
     * window. One of the following values:
     * <ul>
     * <li><code>VERTICAL_ALIGN_TOP</code></li>
     * <li><code>VERTICAL_ALIGN_CENTER</code></li>
     * <li><code>VERTICAL_ALIGN_BOTTOM</code></li>
     * </ul>
     */
    int mVerticalAlign;
    
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
