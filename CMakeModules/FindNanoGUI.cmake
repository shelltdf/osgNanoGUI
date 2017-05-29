
#  CEF_FOUND
#  CEF_INCLUDE_DIR
#  CEF_LIBRARY
#  CEF_LIBRARY_DEBUG
#  CEF_WRAPPER_LIBRARY
#  CEF_WRAPPER_LIBRARY_DEBUG

# $DEF_DIR is an environment variable that would
# correspond to the ./configure --prefix=$DEF_DIR

FIND_PATH( NanoGUI_DIR include/nanogui/nanogui.h )
            
FIND_PATH( NanoGUI_INCLUDE_DIR nanogui/nanogui.h
            ${NanoGUI_DIR}/include
		  ) 
          
          
FIND_PATH( NanoVG_INCLUDE_DIR nanovg.h
            ${NanoGUI_DIR}/ext/nanovg/src
) 

FIND_PATH( GLAD_INCLUDE_DIR glad/glad.h
            ${NanoGUI_DIR}/ext/glad/include
) 

# FIND_LIBRARY(NanoVG_LIBRARY NAMES libcef PATHS ${CEF_DIR}/Release)
# FIND_LIBRARY(NanoVG_LIBRARY_DEBUG NAMES libcef PATHS ${CEF_DIR}/Debug)
		 
FIND_LIBRARY(NanoGUI_LIBRARY NAMES libcef PATHS ${CEF_DIR}/Release)
FIND_LIBRARY(NanoGUI_LIBRARY_DEBUG NAMES libcef PATHS ${CEF_DIR}/Debug)


SET( NanoGUI_FOUND "NO" )
IF( NanoGUI_INCLUDE_DIR)
    SET( NanoGUI_FOUND "YES" )
ENDIF( NanoGUI_INCLUDE_DIR )

