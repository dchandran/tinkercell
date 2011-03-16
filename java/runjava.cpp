#include <iostream>
#include <stdio.h>
#include <string.h>
#include <jni.h>
#include <string>
#include "TC_api.h"

static JNIEnv * JAVAENV = 0;
static JavaVM * JAVAVM = 0;
static std::string previousClassName;
static std::string previousMethodName;
static jclass previousClass = NULL;
static jmethodID previousMethod = NULL;

JNIEnv* create_java_vm(JavaVM ** jvm, const char * classpath)
{
    JNIEnv *env;
    JavaVMInitArgs vm_args;
    JavaVMOption options[3];
    std::string path1("-Djava.class.path=."), 
    				 extra(classpath),
    				 path2("-Djava.library.path=.");
    if (!extra.empty())
    {
    	path1.append(":");
    	path1.append(extra);
    	path2.append(":");
    	path2.append(extra);
    }
    
    char * c1 = new char[ path1.size() ];
    char * c2 = new char[ path2.size() ];
    
    strcpy(c1, path1.c_str());
    strcpy(c2, path2.c_str());
    
    options[0].optionString = c1;
    options[1].optionString = c2;
    vm_args.version = JNI_VERSION_1_6; //JDK version. This indicates version 1.6
    vm_args.options = options;
    vm_args.nOptions = 2;
    vm_args.ignoreUnrecognized = JNI_TRUE;
    
    int ret = JNI_CreateJavaVM(jvm, (void**)&env, &vm_args);
    
    delete c1;
    delete c2;

    if(ret < 0) return 0; //error
    return env;
}

extern "C"
{
	TCAPIEXPORT int initialize(const char * classpath)
	{
		JAVAENV = create_java_vm(&JAVAVM, classpath);
		if (JAVAENV == NULL) return 0;
		return 1;
	}

	TCAPIEXPORT int exec(const char * classname, const char * methodname, const char * arg)
	{
		if (JAVAENV == NULL) return 0;

		jclass currentClass = NULL;
		jmethodID currentMethod = NULL;
		std::string currentClassName(classname);
		std::string currentMethodName(methodname);

		if ((previousClassName.compare(currentClassName)==0) && (previousClass != NULL))
		{
			currentClass = previousClass;
			if ((previousMethodName.compare(currentMethodName)==0) && (previousMethod != NULL))
				currentMethod = previousMethod;
		}
	    
		//Obtaining Classes
		if (currentClass == NULL)
			currentClass = JAVAENV->FindClass(classname);
	    
		//Obtaining Method IDs
		if (currentClass != NULL && currentMethod == NULL)
			currentMethod = JAVAENV->GetStaticMethodID(currentClass,methodname,"(Ljava/lang/String;)V");

		if (currentClass != NULL && currentMethod != NULL)
		{
			jstring StringArg = JAVAENV->NewStringUTF(arg);
			//Calling another static method and passing string type parameter
			JAVAENV->CallStaticVoidMethod(currentClass,currentMethod,StringArg);
			return 1;
		}
		
		if (currentClass == NULL)
			return -1;
		
		if (currentMethod == NULL)
			return -2;
	}

	TCAPIEXPORT void finalize()
	{
		if (JAVAVM)
			int n = JAVAVM->DestroyJavaVM();
	}
}

