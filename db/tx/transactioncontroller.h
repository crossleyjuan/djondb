/*
 * =====================================================================================
 *
 *       Filename:  transactioncontroller.h
 *
 *    Description:  This header defines the transaction controller, its main function
 *                  is the coordination of other nodes
 *
 *        Version:  1.0
 *        Created:  12/03/2012 10:20:32 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Juan Pablo Crossley (Cross), crossleyjuan@gmail.com
 *   Organization:  djondb
 *
 * This file is part of the djondb project, for license information please refer to the LICENSE file,
 * the application and libraries are provided as-is and free of use under the terms explained in the file LICENSE
 * Its authors create this application in order to make the world a better place to live, but you should use it on
 * your own risks.
 * 
 * Also, be adviced that, the GPL license force the committers to ensure this application will be free of use, thus
 * if you do any modification you will be required to provide it for free unless you use it for personal use (you may 
 * charge yourself if you want), bare in mind that you will be required to provide a copy of the license terms that ensures
 * this program will be open sourced and all its derivated work will be too.
 * =====================================================================================
 */ 
#ifndef TRANSACTIONCONTROLLER_INCLUDED_H
#define TRANSACTIONCONTROLLER_INCLUDED_H 

#include "transactiondefs.h"

class TransactionController {

	public:
		bool notifyNodes(TransactionOperation* operation);
		bool receiveNotification(TransactionOperation* operation);

	private:
};

#endif /* TRANSACTIONCONTROLLER_INCLUDED_H */
