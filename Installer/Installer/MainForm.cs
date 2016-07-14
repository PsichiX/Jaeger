using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace Installer
{
    public partial class MainForm : Form
    {
        const string INSTALLATION_DIR = "installation-dir";

        public MainForm()
        {
            InitializeComponent();
            ResetPath();
        }

        private void ResetPath()
        {
            pathTextBox.Text = Path.Combine(Environment.GetFolderPath(Environment.SpecialFolder.ProgramFiles), "jaeger");
        }

        private void DirectoryCopy(string sourceDirName, string destDirName, bool copySubDirs)
        {
            DirectoryInfo dir = new DirectoryInfo(sourceDirName);
            if (!dir.Exists)
                throw new DirectoryNotFoundException("Source directory does not exist or could not be found: " + sourceDirName);
            DirectoryInfo[] dirs = dir.GetDirectories();
            if (!Directory.Exists(destDirName))
                Directory.CreateDirectory(destDirName);
            FileInfo[] files = dir.GetFiles();
            foreach (FileInfo file in files)
            {
                string temppath = Path.Combine(destDirName, file.Name);
                file.CopyTo(temppath, true);
            }
            if (copySubDirs)
            {
                foreach (DirectoryInfo subdir in dirs)
                {
                    string temppath = Path.Combine(destDirName, subdir.Name);
                    DirectoryCopy(subdir.FullName, temppath, copySubDirs);
                }
            }
        }

        private void RunShell(string cmd)
        {
            System.Diagnostics.Process process = new System.Diagnostics.Process();
            System.Diagnostics.ProcessStartInfo startInfo = new System.Diagnostics.ProcessStartInfo();
            startInfo.WindowStyle = System.Diagnostics.ProcessWindowStyle.Hidden;
            startInfo.FileName = "cmd.exe";
            startInfo.Arguments = "/C " + cmd;
            process.StartInfo = startInfo;
            process.Start();
            process.WaitForExit();
        }

        private void selectButton_Click(object sender, EventArgs e)
        {
            if (string.IsNullOrEmpty(pathTextBox.Text))
                ResetPath();
            var fbd = new FolderBrowserDialog();
            fbd.Description = "Select installation destination.";
            fbd.ShowNewFolderButton = true;
            fbd.SelectedPath = pathTextBox.Text;
            fbd.ShowDialog();
        }

        private void installButton_Click(object sender, EventArgs e)
        {
            if (installCheckBox.Checked && Directory.Exists(INSTALLATION_DIR))
                DirectoryCopy(INSTALLATION_DIR, pathTextBox.Text, true);
            if (addToPathCheckBox.Checked)
            {
                var installationPath = Path.GetFullPath(pathTextBox.Text);
                var path = Environment.GetEnvironmentVariable("PATH", EnvironmentVariableTarget.Machine);
                if (!path.Split(';').Contains(installationPath))
                    Environment.SetEnvironmentVariable("PATH", path + ";" + installationPath, EnvironmentVariableTarget.Machine);
            }
            if (registerStdCheckBox.Checked)
            {
                var stdPath = Path.Combine(Path.GetFullPath(pathTextBox.Text), "std") + Path.DirectorySeparatorChar;
                Environment.SetEnvironmentVariable("JAEGER_STD", stdPath, EnvironmentVariableTarget.Machine);
            }
            if (associateFileTypesCheckBox.Checked)
            {
                var bin = Path.Combine(Path.GetFullPath(pathTextBox.Text), "jaeger.exe");
                RunShell("assoc .jg=JaegerScript");
                RunShell("ftype JaegerScript=\"" + bin + "\" \"%1\" %*");
            }
            if (shebangCheckBox.Checked)
            {
                var bin = Path.Combine(Path.GetFullPath(pathTextBox.Text), "shebang.exe");
                var pathext = Environment.GetEnvironmentVariable("PATHEXT", EnvironmentVariableTarget.Machine);
                if (!pathext.Split(';').Contains("."))
                    Environment.SetEnvironmentVariable("PATHEXT", pathext + ";.", EnvironmentVariableTarget.Machine);
                RunShell("assoc .=Shebang");
                RunShell("ftype Shebang=\"" + bin + "\" \"%1\" %*");
            }
            MessageBox.Show("Installation complete!");
            Close();
        }
    }
}
