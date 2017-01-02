//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// This is a part of the Litestep Shell source code.
//
// Copyright (C) 1997-2015  LiteStep Development Team
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

using System;
using System.IO;
using System.Reflection;
using System.Runtime.InteropServices;
using System.Text;

using NUnit.Framework;

using static LsapiSharp.NativeMethods;

namespace LsapiSharp.Test
{
    [TestFixture]
    [Author("Donelle Sanders Jr", "donelle@donellesandersjr.com")]
    public class LsapiSharpTest 
    {
        const int MAX_LINE_LENGTH = 4096;
        const int MAX_COMMAND = 64;

        static string stepRCPath;

        [OneTimeSetUp()]
        public static void Initialize()
        {
            var location = Path.GetDirectoryName(Assembly.GetExecutingAssembly().Location);
            var initialized = LSAPIInitialize(location, stepRCPath = Path.Combine(location, "step.rc"));
            Assert.IsTrue(initialized);
        }

        [TestCase]
        [Category("Configuration")]
        public void LCOpen_LCClose_Test()
        {
            IntPtr hfile = LCOpen(null);
            Assert.NotZero(hfile.ToInt64());

            var closed = LCClose(hfile);
            Assert.IsTrue(closed);
        }

        [TestCase]
        [Category("Configuration")]
        public void LCTokenize_Test()
        {
            const string LINE = "*Setting option1 option2 option3 option4 somemore stuff";

            IntPtr hfile = LCOpen(stepRCPath);
            Assert.AreNotEqual(0, hfile.ToInt64());

            string[] tokens = new string[5];
            StringBuilder extraConfig = new StringBuilder(25);

            int len = Tokenize(LINE, tokens, extraConfig);
            string extraParams = extraConfig.ToString();

            Assert.AreEqual(tokens.Length, len);
            Assert.AreEqual("*Setting", tokens[0]);
            Assert.AreEqual("option1", tokens[1]);
            Assert.AreEqual("option2", tokens[2]);
            Assert.AreEqual("option3", tokens[3]);
            Assert.AreEqual("option4", tokens[4]);
            Assert.IsTrue(extraParams.StartsWith("somemore"));

            var closed = LCClose(hfile);
            Assert.IsTrue(closed);
        }

        [TestCase]
        [Category("Configuration")]
        public void LCReadNextCommand_Test()
        {
            IntPtr hfile = LCOpen(stepRCPath);
            Assert.AreNotEqual(0, hfile.ToInt64());

            StringBuilder buffer = new StringBuilder(MAX_LINE_LENGTH);
            bool retval = LCReadNextCommand(hfile, buffer, MAX_LINE_LENGTH);
            string line = buffer.ToString();

            Assert.IsTrue(retval);
            Assert.NotZero(line.Length);
            
            var closed = LCClose(hfile);
            Assert.IsTrue(closed);
        }

        [TestCase]
        [Category("Configuration")]
        public void LCReadNextConfig_Test()
        {
            IntPtr hfile = LCOpen(stepRCPath);
            Assert.AreNotEqual(0, hfile.ToInt64());

            StringBuilder buffer = new StringBuilder(MAX_LINE_LENGTH);
            bool retval = LCReadNextConfig(hfile, "*Config", buffer, MAX_LINE_LENGTH);
            Assert.IsTrue(retval);

            string line = buffer.ToString();
            string [] tokens = new string[4];
            int len = Tokenize(line, tokens);

            Assert.AreEqual(tokens.Length, len);
            Assert.AreEqual("*Config", tokens[0]);
            Assert.AreEqual("option1", tokens[1]);
            Assert.AreEqual("option2", tokens[2]);
            Assert.AreEqual("option3", tokens[3]);

            tokens = new string[2];

            retval = LCReadNextConfig(hfile, "*Config", buffer, MAX_LINE_LENGTH);
            Assert.IsTrue(retval);

            line = buffer.ToString();
            len = Tokenize(line, tokens);

            Assert.AreEqual(tokens.Length, len);
            Assert.AreEqual("*Config", tokens[0]);
            Assert.AreEqual("option1", tokens[1]);
            
            var closed = LCClose(hfile);
            Assert.IsTrue(closed);
        }

        [TestCase]
        [Category("Configuration")]
        public void LCReadNextLine_Test()
        {
            IntPtr hfile = LCOpen(stepRCPath);
            Assert.AreNotEqual(0, hfile.ToInt64());

            StringBuilder buffer = new StringBuilder(MAX_LINE_LENGTH);
            bool retval = LCReadNextLine(hfile, buffer, MAX_LINE_LENGTH);
            string line = buffer.ToString();

            Assert.IsTrue(retval);
            Assert.NotZero(line.Length);

            var closed = LCClose(hfile);
            Assert.IsTrue(closed);
        }

        [TestCase]
        [Category("Configuration")]
        public void GetRCInt_Test()
        {
            IntPtr hfile = LCOpen(stepRCPath);
            Assert.AreNotEqual(0, hfile.ToInt64());

            int retval = GetRCInt("VarB", 0);
            Assert.AreEqual(15, retval);

            retval = GetRCInt("SomeSetting", 0);
            Assert.AreEqual(0, retval);

            long retval64 = GetRCInt64("VarD", 0);
            Assert.AreEqual(9223372036854775800L, retval64);

            retval64 = GetRCInt("SomeSetting", 0);
            Assert.AreEqual(0, retval64);

            var closed = LCClose(hfile);
            Assert.IsTrue(closed);
        }

        [TestCase]
        [Category("Configuration")]
        public void GetRCFloat_Test()
        {
            IntPtr hfile = LCOpen(stepRCPath);
            Assert.AreNotEqual(0, hfile.ToInt64());

            float retval = GetRCFloat("VarF", 0);
            Assert.AreEqual(53.675f, retval);

            retval = GetRCFloat("SomeSetting", 0);
            Assert.AreEqual(0, retval);

            var closed = LCClose(hfile);
            Assert.IsTrue(closed);
        }

        [TestCase]
        [Category("Configuration")]
        public void GetRCDouble_Test()
        {
            IntPtr hfile = LCOpen(stepRCPath);
            Assert.AreNotEqual(0, hfile.ToInt64());

            double retval = GetRCDouble("VarG", 0);
            Assert.AreEqual(1.7E+3d, retval);

            retval = GetRCFloat("SomeSetting", 0);
            Assert.AreEqual(0, retval);

            var closed = LCClose(hfile);
            Assert.IsTrue(closed);
        }

        [TestCase]
        [Category("Configuration")]
        public void GetRCBoolDef_Test()
        {
            IntPtr hfile = LCOpen(stepRCPath);
            Assert.AreNotEqual(0, hfile.ToInt64());

            bool retval = GetRCBoolDef("VarA", false);
            Assert.IsTrue(retval);

            retval = GetRCBoolDef("VarH", false);
            Assert.IsTrue(retval);

            retval = GetRCBoolDef("SomeSetting", false);
            Assert.IsFalse(retval);

            var closed = LCClose(hfile);
            Assert.IsTrue(closed);
        }

        [TestCase]
        [Category("Configuration")]
        public void GetRCBool_Test()
        {
            IntPtr hfile = LCOpen(stepRCPath);
            Assert.AreNotEqual(0, hfile.ToInt64());

            bool retval = GetRCBool("VarA", false);
            Assert.IsFalse(retval);

            retval = GetRCBool("VarA", true);
            Assert.IsTrue(retval);

            retval = GetRCBool("SomeSetting", false);
            Assert.IsTrue(retval);

            var closed = LCClose(hfile);
            Assert.IsTrue(closed);
        }

        [TestCase]
        [Category("Configuration")]
        public void GetRCString_Test()
        {
            IntPtr hfile = LCOpen(stepRCPath);
            Assert.AreNotEqual(0, hfile.ToInt64());

            StringBuilder buffer = new StringBuilder(MAX_COMMAND);
            bool retval = GetRCString("VarC", buffer, "Test", MAX_COMMAND);
            string rcVal = buffer.ToString();

            Assert.IsTrue(retval);
            Assert.AreEqual("Command3", rcVal);

            retval = GetRCString("SomeSetting", buffer, "Test", MAX_COMMAND);
            rcVal = buffer.ToString();

            Assert.IsFalse(retval);
            Assert.AreEqual("Test", rcVal);

            var closed = LCClose(hfile);
            Assert.IsTrue(closed);
        }

        [TestCase]
        [Category("Configuration")]
        public void GetRCLine_Test()
        {
            IntPtr hfile = LCOpen(stepRCPath);
            Assert.AreNotEqual(0, hfile.ToInt64());

            StringBuilder buffer = new StringBuilder(MAX_LINE_LENGTH);
            bool retval = GetRCLine("*Script", buffer, MAX_LINE_LENGTH, null);

            Assert.IsTrue(retval);

            string[] tokens = new string[6];
            Tokenize(buffer.ToString(), tokens);

            Assert.AreEqual("exec", tokens[0]);
            Assert.AreEqual("!LabelSetAlpha", tokens[1]);
            Assert.AreEqual("Initialising", tokens[2]);
            Assert.AreEqual("250", tokens[3]);
            Assert.AreEqual("3", tokens[4]);
            Assert.AreEqual("10", tokens[5]);

            var closed = LCClose(hfile);
            Assert.IsTrue(closed);
        }

        [TestCase]
        [Category("Configuration")]
        public void GetRCColor_Test()
        {
            IntPtr hfile = LCOpen(stepRCPath);
            Assert.AreNotEqual(0, hfile.ToInt64());

            var defaultVal = new COLORREF(0);
            COLORREF retval = GetRCColor("VarJ", defaultVal);

            Assert.AreEqual(70, retval.R);
            Assert.AreEqual(130, retval.G);
            Assert.AreEqual(180, retval.B);
            Assert.AreEqual(11829830, retval.Value);

            retval = GetRCColor("SomethingElse", defaultVal);

            Assert.AreEqual(defaultVal.Value, retval.Value);

            var closed = LCClose(hfile);
            Assert.IsTrue(closed);
        }

        private static int Tokenize(string line, string [] tokens, StringBuilder extra = null)
        {
            int intPtrSize = Marshal.SizeOf(typeof(IntPtr));
            IntPtr tokensPtr = Marshal.AllocHGlobal(intPtrSize * tokens.Length);
            for (int i = 0; i < tokens.Length; i++)
            {
                IntPtr ptr = Marshal.AllocHGlobal(Marshal.SizeOf(typeof(char)) * MAX_COMMAND);
                Marshal.WriteIntPtr(tokensPtr, i * intPtrSize, ptr);
            }

            int tokenLen = LCTokenize(line, tokensPtr, tokens.Length, extra);

            for (int i = 0; i < tokens.Length; i++)
            {
                IntPtr ptr = Marshal.ReadIntPtr(tokensPtr, i * intPtrSize);
                tokens[i] = Marshal.PtrToStringUni(ptr);
                Marshal.FreeHGlobal(ptr);
            }

            return tokenLen;
        }
    }
}
