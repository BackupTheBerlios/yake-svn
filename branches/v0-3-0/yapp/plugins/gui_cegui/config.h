#ifndef _YAPP_GUI_CEGUI_CONFIG_H_
#define _YAPP_GUI_CEGUI_CONFIG_H_

/* compile time flags */
// dll import/export
#if defined(YAPP_GUI_CEGUI_EXPORTS)
#	define YAPP_GUI_CEGUI_API DLLEXPORT
#else
#	define YAPP_GUI_CEGUI_API DLLIMPORT
#endif

#endif // _YAPP_GUI_CEGUI_CONFIG_H_