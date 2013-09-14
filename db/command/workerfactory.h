// =====================================================================================
// 
//  @file:  workerfactory.h
// 
//  @brief:  Defines the factory for worker processes, using this a command will be turned
//  into a worker process, which could be executed later by the worker engine
// 
//  @version:  1.0
//  @date:     07/21/2013 04:56:34 AM
//  Compiler:  g++
// 
//  @author:  Juan Pablo Crossley (Cross), crossleyjuan@gmail.com
// 
// This file is part of the djondb project, for license information please refer to the LICENSE file,
// the application and libraries are provided as-is and free of use under the terms explained in the file LICENSE
// Its authors create this application in order to make the world a better place to live, but you should use it on
// your own risks.
// 
// Also, be adviced that, the GPL license force the committers to ensure this application will be free of use, thus
// if you do any modification you will be required to provide it for free unless you use it for personal use (you may 
// charge yourself if you want), bare in mind that you will be required to provide a copy of the license terms that ensures
// this program will be open sourced and all its derivated work will be too.
// =====================================================================================
#ifndef WORKERFACTORY_INCLUDED_H
#define WORKERFACTORY_INCLUDED_H 

class Worker;
class InputStream;
class OutputStream;
class Command;

Worker* getWorker(Command* command, InputStream* input, OutputStream* output);

#endif /* WORKERFACTORY_INCLUDED_H */
