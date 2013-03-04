#pragma once

#include "service.h"
#include "util.h"

using namespace System;
using namespace System::Collections;
using namespace System::ServiceProcess;
using namespace System::ComponentModel;


namespace djondb_service {

	/// <summary>
	/// Summary for djondb_serviceWinService
	/// </summary>
	///
	/// WARNING: If you change the name of this class, you will need to change the
	///          'Resource File Name' property for the managed resource compiler tool
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	public ref class djondb_serviceWinService : public System::ServiceProcess::ServiceBase
	{
		bool _stopRequested;
	public:
		djondb_serviceWinService()
		{
			InitializeComponent();
			_stopRequested = false;
			//
			//TODO: Add the constructor code here
			//
		}
	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~djondb_serviceWinService()
		{
			if (components)
			{
				delete components;
			}
		}

		/// <summary>
		/// Set things in motion so your service can do its work.
		/// </summary>
		virtual void OnStart(array<String^>^ args) override
		{
			Logger* log = getLogger(NULL);
			log->info("djondbd version %s is starting up.", VERSION);
			service_startup();

			System::Threading::Thread^ thread = gcnew System::Threading::Thread(gcnew System::Threading::ParameterizedThreadStart(djondb_service::djondb_serviceWinService::monitorService));
			thread->Start(this);
		}

		static void monitorService( System::Object^ arg) {
			djondb_serviceWinService^ service = (djondb_serviceWinService^)arg;

			while (service_running()) {
				Thread::sleep(3000);
			}
			// If the service was not shutdown by windows the it was by command
			if (!service->_stopRequested) {
				service->Stop();
			}
		}

		/// <summary>
		/// Stop this service.
		/// </summary>
		virtual void OnStop() override
		{
			_stopRequested = true;
			if (service_running()) {
				service_shutdown();
			}
		}

	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>
		System::ComponentModel::Container ^components;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->components = gcnew System::ComponentModel::Container();
			this->CanStop = true;
			this->CanPauseAndContinue = true;
			this->AutoLog = true;
			this->ServiceName = L"djondb_serviceWinService";
		}

#pragma endregion

	};
}
