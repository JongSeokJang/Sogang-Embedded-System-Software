LOCAL_PATH:=$(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE:=dangercloz_module
LOCAL_SRC_FILES:=TextEditor.c FigureSwitch.c PuzzleCount.c
LOCAL_LDLIBS := -llog
#LOCAL_LDLIB := -L$(SYSROOT)/usr/lib -llog

include $(BUILD_SHARED_LIBRARY)

