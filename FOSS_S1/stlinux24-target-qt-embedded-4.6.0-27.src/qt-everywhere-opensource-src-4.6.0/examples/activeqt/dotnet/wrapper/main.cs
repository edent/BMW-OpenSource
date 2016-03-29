//! [0]
using System;

namespace WrapperApp
{
	class App
	{
		void Run()
		{
			netWorker worker = new netWorker();

			worker.statusStringChanged += new netWorker.__Delegate_statusStringChanged(onStatusStringChanged);

			System.Console.Out.WriteLine(worker.StatusString);

			System.Console.Out.WriteLine("Working cycle begins...");
			worker.StatusString = "Working";
			worker.StatusString = "Lunch Break";
			worker.StatusString = "Working";
			worker.StatusString = "Idle";
			System.Console.Out.WriteLine("Working cycle ends...");
		}

		private void onStatusStringChanged(string str)
		{
			System.Console.Out.WriteLine(str);
		}

		[STAThread]
//! [0] //! [1]
		static void Main(string[] args)
		{
			App app = new App();
			app.Run();
		}
//! [1] //! [2]
	}
//! [2] //! [3]
}
//! [3]
