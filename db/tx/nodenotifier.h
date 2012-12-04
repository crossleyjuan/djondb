/*
 * =====================================================================================
 *
 *       Filename:  nodenotifier.h
 *
 *    Description:  This defines the header of Node Notifiers, which will send notifications
 *                  to other nodes about new TransactionOperations
 *
 *        Version:  1.0
 *        Created:  12/03/2012 11:15:42 PM
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
#ifndef NODENOTIFIER_INCLUDED_H
#define NODENOTIFIER_INCLUDED_H 

#include "networkoutputstream.h"
#include "networkinputstream.h"
#include "util.h"
#include <queue.h>

class NodeNotifier {
	public:
		NodeNotifier();
		NodeNotifier(const NodeNotifier& orig);
		virtual ~NodeNotifier();

		void sendNotification(TransactionOperation* oper);
		void addNode(std::string host, int port = 2314);

	private:
		Thread _thread;
		std::list<NetworkOutputStream*> _nodes;
		std::queue<TransactionOperations*> _operations;
};
#endif /* NODENOTIFIER_INCLUDED_H */
