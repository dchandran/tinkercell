#include <stdio.h>
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
    JavaVMOption options;
    std::string path1("-Djava.class.path=."), 
    				 path2(classpath);
    if (!path2.empty())
    {
    	path1.append(";");
    	path1.append(path2);
    }
    options.optionString = const_cast<char*>(path1.c_str()); //Path to the java source code
    vm_args.version = JNI_VERSION_1_6; //JDK version. This indicates version 1.6
    vm_args.nOptions = 1;
    vm_args.options = &options;
    vm_args.ignoreUnrecognized = 0;
    
    int ret = JNI_CreateJavaVM(jvm, (void**)&env, &vm_args);
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

		return 0;
	}

	TCAPIEXPORT void finalize()
	{
		if (JAVAVM)
			int n = JAVAVM->DestroyJavaVM();
	}
}

