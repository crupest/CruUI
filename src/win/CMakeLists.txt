set(CRU_WIN_BASE_INCLUDE_DIR ${CRU_INCLUDE_DIR}/cru/win/)

add_library(cru_win_base STATIC
	DebugLogger.hpp
	StdOutLogger.hpp

	Exception.cpp
	HeapDebug.cpp
)
target_sources(cru_win_base PUBLIC
	${CRU_WIN_BASE_INCLUDE_DIR}/Exception.hpp
	${CRU_WIN_BASE_INCLUDE_DIR}/WinPreConfig.hpp
)
target_compile_definitions(cru_win_base PUBLIC UNICODE _UNICODE) # use unicode
target_link_libraries(cru_win_base PUBLIC cru_base)

add_subdirectory(graphics)
add_subdirectory(gui)
