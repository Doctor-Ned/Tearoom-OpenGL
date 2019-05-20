# Add source files
file(GLOB_RECURSE SOURCE_FILES 
	 *.c
	 *.cpp)
	
# Add header files
file(GLOB_RECURSE HEADER_FILES 
	 *.h
	 *.hpp)

set(SHADER_DIR "Shaders")

file(GLOB_RECURSE SHADER_FILES
	${SHADER_DIR}/*.glsl
	${SHADER_DIR}/*.sglsl)
	 
file(COPY ${FREETYPE_DIR}/freetype.dll
	 DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
	 
file(COPY ${IRRKLANG_DIR}/ikpMP3.dll
	 DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
	 
file(COPY ${IRRKLANG_DIR}/irrKlang.dll
	 DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

# Define the executable
add_executable(${PROJECT_NAME} ${SHADER_FILES} ${HEADER_FILES} ${SOURCE_FILES})
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 11)

# Define the include DIRs
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(${PROJECT_NAME} PUBLIC "${ASSIMP_INCLUDE_DIR}")
target_include_directories(${PROJECT_NAME} PUBLIC "${GLFW_INCLUDE_DIR}")
target_include_directories(${PROJECT_NAME} PUBLIC "${GLAD_INCLUDE_DIR}")
target_include_directories(${PROJECT_NAME} PUBLIC "${GLM_INCLUDE_DIR}")
target_include_directories(${PROJECT_NAME} PUBLIC "${IMGUI_INCLUDE_DIR}")
target_include_directories(${PROJECT_NAME} PUBLIC "${STB_IMAGE_INCLUDE_DIR}")
target_include_directories(${PROJECT_NAME} PUBLIC "${FREETYPE_INCLUDE_DIRS}")
target_include_directories(${PROJECT_NAME} PUBLIC "${IRRKLANG_INCLUDE_DIRS}")

target_link_libraries(${PROJECT_NAME} "${OPENGL_LIBRARY}")
target_link_libraries(${PROJECT_NAME} "${ASSIMP_LIBRARY}")
target_link_libraries(${PROJECT_NAME} "${GLFW_LIBRARY}")
target_link_libraries(${PROJECT_NAME} "${GLAD_LIBRARY}"      "${CMAKE_DL_LIBS}")
target_link_libraries(${PROJECT_NAME} "${IMGUI_LIBRARY}"     "${CMAKE_DL_LIBS}")
target_link_libraries(${PROJECT_NAME} "${STB_IMAGE_LIBRARY}" "${CMAKE_DL_LIBS}")
target_link_libraries(${PROJECT_NAME} "${FREETYPE_LIBRARIES}")
target_link_libraries(${PROJECT_NAME} "${IRRCLANG_LIBRARY}")

target_compile_definitions(${PROJECT_NAME} PRIVATE GLFW_INCLUDE_NONE)
target_compile_definitions(${PROJECT_NAME} PRIVATE LIBRARY_SUFFIX="")

add_custom_command(TARGET  ${PROJECT_NAME} POST_BUILD
				   COMMAND ${CMAKE_COMMAND} -E copy_directory
						   ${CMAKE_SOURCE_DIR}/res
						   ${CMAKE_CURRENT_BINARY_DIR}/res)
						   
#add_custom_command(TARGET  ${PROJECT_NAME} POST_BUILD
#				   COMMAND ${CMAKE_COMMAND} -E copy_directory
#						   ${CMAKE_CURRENT_BINARY_DIR}/Scenes
#						   ${CMAKE_SOURCE_DIR}/Scenes)
						   
#add_custom_command(TARGET  ${PROJECT_NAME} POST_BUILD
#				   COMMAND ${CMAKE_COMMAND} -E copy_directory
#						   ${CMAKE_SOURCE_DIR}/Scenes
#						   ${CMAKE_CURRENT_BINARY_DIR}/Scenes)
						   
add_custom_command(TARGET  ${PROJECT_NAME} POST_BUILD
				   COMMAND ${CMAKE_COMMAND} -E copy_directory
						   ${CMAKE_SOURCE_DIR}/src/${SHADER_DIR}
						   ${CMAKE_CURRENT_BINARY_DIR}/${SHADER_DIR})						   

# Create virtual folders to make it look nicer in VS
if(MSVC_IDE)
	# Macro to preserve source files hierarchy in the IDE
	macro(GroupSources curdir)
		file(GLOB children RELATIVE ${CMAKE_SOURCE_DIR}/${curdir} ${CMAKE_SOURCE_DIR}/${curdir}/*)

		foreach(child ${children})
			if(IS_DIRECTORY ${CMAKE_SOURCE_DIR}/${curdir}/${child})
				GroupSources(${curdir}/${child})
			else()
				string(REPLACE "/" "\\" groupname ${curdir})
				set(CURRENT_CHILD ${CMAKE_SOURCE_DIR}/${curdir}/${child})
				if(CURRENT_CHILD MATCHES .*\.h$)
					string(REPLACE "src" "Headers" groupname ${groupname})
				elseif(CURRENT_CHILD MATCHES .*\.glsl$)
					string(REPLACE "src\\" "" groupname ${groupname})
				else()
					string(REPLACE "src" "Sources" groupname ${groupname})
				endif()
				source_group(${groupname} FILES ${CMAKE_SOURCE_DIR}/${curdir}/${child})
			endif()
		endforeach()
	endmacro()
	
	# Run macro
	GroupSources(src)
endif()