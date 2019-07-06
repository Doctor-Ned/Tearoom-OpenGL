project(Shaders)

file(GLOB_RECURSE SHADER_FILES
	*.glsl
	*.sglsl)
	
# i have no idea how to make it properly and don't feel like spending my whole life on finding out xd
add_library(${PROJECT_NAME} ${SHADER_FILES} dummy.h dummy.cpp)

if(MSVC_IDE)
	
	macro(GroupShaders curdir)
		file(GLOB children RELATIVE ${CMAKE_SOURCE_DIR}/${curdir} ${CMAKE_SOURCE_DIR}/${curdir}/*)

		foreach(child ${children})
			if(IS_DIRECTORY ${CMAKE_SOURCE_DIR}/${curdir}/${child})
				GroupShaders(${curdir}/${child})
			else()
				string(REPLACE "/" "\\" groupname ${curdir})
				set(CURRENT_CHILD ${CMAKE_SOURCE_DIR}/${curdir}/${child})
				if(CURRENT_CHILD MATCHES .*\.glsl$)
					string(REPLACE "Shaders\\" "" groupname ${groupname})
					string(REPLACE "Shaders" "" groupname ${groupname})
				elseif(CURRENT_CHILD MATCHES .*\.h$ OR CURRENT_CHILD MATCHES .*\.cpp$)
					string(REPLACE "Shaders" "X" groupname ${groupname})
				else()
					string(REPLACE "Shaders" "" groupname ${groupname})
				endif()
				source_group("${groupname}" FILES ${CMAKE_SOURCE_DIR}/${curdir}/${child})
			endif()
		endforeach()
	endmacro()
	
	GroupShaders(${SHADER_DIR})
endif()