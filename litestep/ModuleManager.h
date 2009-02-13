//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// This is a part of the Litestep Shell source code.
//
// Copyright (C) 1997-2009  LiteStep Development Team
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
#if !defined(MODULEMANAGER_H)
#define MODULEMANAGER_H

#include "module.h"
#include "../utility/IManager.h"
#include "../utility/common.h"
#include <list>


/** List of modules */
typedef std::list<Module*> ModuleQueue;


/**
 * Manages loaded modules.
 */
class ModuleManager: public IManager
{
public:
    /**
     * Constructor.
     */
    ModuleManager();
    
    /**
     * Destructor.
     */
    virtual ~ModuleManager();
    
    /**
     * Starts the module manager and loads all the modules specified in
     * <code>step.rc</code>.
     *
     * @param  pILiteStep  interface to LiteStep's core
     * @return <code>S_OK</code> if successful or an error code
     */
    HRESULT Start(ILiteStep *pILiteStep);
    
    /**
     * Unloads all modules and stops the module manager.
     *
     * @return <code>S_OK</code> if successful or an error code
     */
    HRESULT Stop();
    
    /**
     * Loads all the modules specified in <code>step.rc</code>.
     *
     * @return <code>S_OK</code> if successful or an error code
     */
    HRESULT rStart();
    
    /**
     * Unloads all modules.
     *
     * @return <code>S_OK</code> if successful or an error code
     */
    HRESULT rStop();
    
    /**
     * Loads a module.
     *
     * @param  pszLocation  path to the module's DLL
     * @param  dwFlags      set of flags that control how the module is loaded
     * @return <code>TRUE</code> if successful or <code>FALSE</code> if an
     *         error occurs
     */
    BOOL LoadModule(LPCSTR pszLocation, DWORD dwFlags);
    
    /**
     * Unloads a module given its instance handle.
     *
     * @param  hModule  handle to the module's DLL instance
     * @return <code>TRUE</code> if successful or <code>FALSE</code> if an
     *         error occurs
     */
    BOOL QuitModule(HINSTANCE hModule);
    
    /**
     * Unloads a module given the path to its DLL.
     *
     * @param  pszLocation  path to the module's DLL
     * @return <code>TRUE</code> if successful or <code>FALSE</code> if an
     *         error occurs
     */
    BOOL QuitModule(LPCSTR pszLocation);
    
    /**
     * Reloads a module given its instance handle
     *
     * @param  hModule  handle to the module's DLL instance
     * @return <code>TRUE</code> if successful or <code>FALSE</code> if an
     *         error occurs
     */
    BOOL ReloadModule(HINSTANCE hModule);
    
    /**
     * Enumerates loaded modules. Calls the callback function once for each
     * loaded module. Continues until all modules have been enumerated or the
     * callback function returns <code>FALSE</code>.
     *
     * @param  pfnCallback  pointer to callback function
     * @param  lParam       application-defined value passed to the callback
     *                      function
     * @return <code>S_OK</code> if all modules were enumerated,
     *         <code>S_FALSE</code> if the callback function returned
     *         <code>FALSE</code>, or an error code
     */
    HRESULT EnumModules(LSENUMMODULESPROC pfnCallback, LPARAM lParam) const;
    
private:
    /**
     * Loads all the modules specified in <code>step.rc</code>.
     *
     * @return number of modules loaded
     */
    UINT _LoadModules();
    
    /**
     * Initializes the modules in the specified list.
     *
     * @param  mqModules  list of modules to initialize
     * @return number of modules initialized
     */
    UINT _StartModules(ModuleQueue& mqModules);
    
    /**
     * Unloads all loaded modules.
     */
    void _QuitModules();
    
    /**
     * Finds a module in the loaded module list based on the path to its DLL.
     *
     * @return iterator that points to the module or the end of the list
     */
    ModuleQueue::iterator _FindModule(LPCSTR pszLocation);
    
    /**
     * Finds a module in the loaded module list based on its instance handle.
     *
     * @return iterator that points to the module or the end of the list
     */
    ModuleQueue::iterator _FindModule(HINSTANCE hModule);
    
    /**
     * Allocates a module object.
     *
     * @param  pszLocation  path to the module's DLL
     * @param  dwFlags      set of flags that control how the module is loaded
     * @return pointer to the module object
     */
    Module* _MakeModule(LPCSTR pszLocation, DWORD dwFlags);
    
    /**
     * Waits for an array of events to all be set while remaining responsive
     * to user input.
     *
     * @param  pHandles  array of event handles
     * @param  dwCount   number of handles in the array
     */
    void _WaitForModules(const HANDLE* pHandles, size_t stCount) const;
    
    /** List of loaded modules */
    ModuleQueue m_ModuleQueue;
    
    /** Pointer to LiteStep's core interface */
    ILiteStep *m_pILiteStep;
    
    /** LiteStep's main window handle */
    HWND m_hLiteStep;
    
    /** Path to LiteStep's root directory */
    std::string m_sAppPath;
    
    /**
     * Predicate used by <code>_FindModule</code> to locate a loaded module
     * given the path to its DLL.
     */
    struct IsLocationEqual
    {
        /**
         * Constructor.
         */
        IsLocationEqual(LPCSTR pszLocation) : m_pszLocation(pszLocation)
        {
            // do nothing
        }
        
        /**
         * Returns <code>true</code> if specified module's DLL path matches
         * the path passed to the constructor.
         */
        bool operator() (const Module* pModule) const
        {
            return (_stricmp(m_pszLocation, pModule->GetLocation()) == 0);
        }
        
    private:
        /** Path to match */
        LPCSTR m_pszLocation;
    };
    
    /**
     * Predicate used by <code>_FindModule</code> to locate a loaded module
     * given its DLL instance handle.
     */
    struct IsInstanceEqual
    {
        /**
         * Constructor.
         */
        IsInstanceEqual(HINSTANCE hInstance) : m_hInstance(hInstance)
        {
            // do nothing
        }
        
        /**
         * Returns <code>true</code> if specified module's DLL instance handle
         * matches the handle passed to the constructor.
         */
        bool operator() (const Module* pModule) const
        {
            return (pModule->GetInstance() == m_hInstance);
        }
        
    private:
        /** Instance handle to match */
        HINSTANCE m_hInstance;
    };
};


#endif // MODULEMANAGER_H
