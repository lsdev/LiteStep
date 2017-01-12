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

using Microsoft.Win32.SafeHandles;
using NUnit.Framework;

using static LsapiSharp.NativeMethods;


namespace LsapiSharp.Test
{
    [TestFixture]
    [Author("Donelle Sanders Jr", "donelle@donellesandersjr.com")]
    public class LsapiSharpTest
    {
        const int MAX_PATH = 256;
        const int MAX_LINE_LENGTH = 4096;
        const int MAX_COMMAND = 64;

        static string stepRCPath;
        static SafeFileHandle hstepRCFile;

        [OneTimeSetUp()]
        public static void InitializeTest()
        {
            var location = Path.GetDirectoryName(Assembly.GetExecutingAssembly().Location);
            var initialized = LSAPIInitialize(location, stepRCPath = Path.Combine(location, "step.rc"));
            Assert.IsTrue(initialized);

            hstepRCFile = LCOpen(stepRCPath);
            Assert.IsFalse(hstepRCFile.IsClosed);
        }

        [OneTimeTearDown]
        public static void FinalizeTest()
        {
            var closed = LCClose(hstepRCFile);
            Assert.IsTrue(closed);
        }
        

        [TestCase]
        [Category("Configuration")]
        public void LCTokenize_Test()
        {
            const string LINE = "*Setting option1 option2 option3 option4 somemore stuff";

            string[] tokens = new string[5];
            StringBuilder extraConfig = new StringBuilder(25);

            int len = Tokenize(LCTokenize, LINE, tokens, extraConfig);
            string extraParams = extraConfig.ToString();

            Assert.AreEqual(tokens.Length, len);
            Assert.AreEqual("*Setting", tokens[0]);
            Assert.AreEqual("option1", tokens[1]);
            Assert.AreEqual("option2", tokens[2]);
            Assert.AreEqual("option3", tokens[3]);
            Assert.AreEqual("option4", tokens[4]);
            Assert.IsTrue(extraParams.StartsWith("somemore"));
        }

        [TestCase]
        [Category("Configuration")]
        public void CommandTokenize_Test()
        {
            const string LINE = "*Setting !commandA [!command1][!command2] somemore stuff";

            string[] tokens = new string[4];
            StringBuilder extraConfig = new StringBuilder(25);

            int len = Tokenize(CommandTokenize, LINE, tokens, extraConfig);
            string extraParams = extraConfig.ToString();

            Assert.AreEqual(tokens.Length, len);
            Assert.AreEqual("*Setting", tokens[0]);
            Assert.AreEqual("!commandA", tokens[1]);
            Assert.AreEqual("!command1", tokens[2]);
            Assert.AreEqual("!command2", tokens[3]);
            Assert.IsTrue(extraParams.StartsWith("somemore"));
        }

        [TestCase]
        [Category("Configuration")]
        public void CommandParse_Test()
        {
            string line = "[!command1][!command2][!command3 !command4]";

            string cmd = "!command1";
            StringBuilder cmdbuffer = new StringBuilder(cmd.Length + 1),
                          argbuffer = new StringBuilder(line.Length + 1);

            CommandParse(line, cmdbuffer, argbuffer, cmdbuffer.Capacity, argbuffer.Capacity);
            Assert.AreEqual(cmd, cmdbuffer.ToString());

            line = argbuffer.ToString();
            cmd = "!command2";
            cmdbuffer = new StringBuilder(cmd.Length + 1);
            argbuffer = new StringBuilder(line.Length + 1);

            CommandParse(line, cmdbuffer, argbuffer, cmdbuffer.Capacity, argbuffer.Capacity);
            Assert.AreEqual(cmd, cmdbuffer.ToString());

            line = argbuffer.ToString();
            cmd = "!command3 !command4";
            cmdbuffer = new StringBuilder(cmd.Length + 1);

            CommandParse(line, cmdbuffer, null, cmdbuffer.Capacity, 0);
            Assert.AreEqual(cmd, cmdbuffer.ToString());
        }

        [TestCase]
        [Category("Configuration")]
        public void LCReadNextCommand_Test()
        {
            StringBuilder buffer = new StringBuilder(MAX_LINE_LENGTH);
            bool retval = LCReadNextCommand(hstepRCFile, buffer, buffer.Capacity);
            string line = buffer.ToString();

            Assert.IsTrue(retval);
            Assert.NotZero(line.Length);
        }

        [TestCase]
        [Category("Configuration")]
        public void LCReadNextConfig_Test()
        {
            StringBuilder buffer = new StringBuilder(MAX_LINE_LENGTH);
            bool retval = LCReadNextConfig(hstepRCFile, "*Config", buffer, buffer.Capacity);
            Assert.IsTrue(retval);

            string line = buffer.ToString();
            string[] tokens = new string[4];
            int len = Tokenize(LCTokenize, line, tokens);

            Assert.AreEqual(tokens.Length, len);
            Assert.AreEqual("*Config", tokens[0]);
            Assert.AreEqual("option1", tokens[1]);
            Assert.AreEqual("option2", tokens[2]);
            Assert.AreEqual("option3", tokens[3]);

            tokens = new string[2];

            retval = LCReadNextConfig(hstepRCFile, "*Config", buffer, buffer.Capacity);
            Assert.IsTrue(retval);

            line = buffer.ToString();
            len = Tokenize(LCTokenize, line, tokens);

            Assert.AreEqual(tokens.Length, len);
            Assert.AreEqual("*Config", tokens[0]);
            Assert.AreEqual("option1", tokens[1]);
        }

        [TestCase]
        [Category("Configuration")]
        public void LCReadNextLine_Test()
        {
            StringBuilder buffer = new StringBuilder(MAX_LINE_LENGTH);
            bool retval = LCReadNextLine(hstepRCFile, buffer, buffer.Capacity);
            string line = buffer.ToString();

            Assert.IsTrue(retval);
            Assert.NotZero(line.Length);
        }

        [TestCase]
        [Category("Configuration")]
        public void GetRCInt_Test()
        {
            int retval = GetRCInt("VarB", 0);
            Assert.AreEqual(15, retval);

            retval = GetRCInt("SomeSetting", 0);
            Assert.AreEqual(0, retval);

            long retval64 = GetRCInt64("VarD", 0);
            Assert.AreEqual(9223372036854775800L, retval64);

            retval64 = GetRCInt("SomeSetting", 0);
            Assert.AreEqual(0, retval64);
        }

        [TestCase]
        [Category("Configuration")]
        public void GetRCFloat_Test()
        {
            float retval = GetRCFloat("VarF", 0);
            Assert.AreEqual(53.675f, retval);

            retval = GetRCFloat("SomeSetting", 0);
            Assert.AreEqual(0, retval);
        }

        [TestCase]
        [Category("Configuration")]
        public void GetRCDouble_Test()
        {
            double retval = GetRCDouble("VarG", 0);
            Assert.AreEqual(1.7E+3d, retval);

            retval = GetRCFloat("SomeSetting", 0);
            Assert.AreEqual(0, retval);
        }

        [TestCase]
        [Category("Configuration")]
        public void GetRCBoolDef_Test()
        {
            bool retval = GetRCBoolDef("VarA", false);
            Assert.IsTrue(retval);

            retval = GetRCBoolDef("VarH", false);
            Assert.IsTrue(retval);

            retval = GetRCBoolDef("SomeSetting", false);
            Assert.IsFalse(retval);
        }

        [TestCase]
        [Category("Configuration")]
        public void GetRCBool_Test()
        {
            bool retval = GetRCBool("VarA", false);
            Assert.IsFalse(retval);

            retval = GetRCBool("VarA", true);
            Assert.IsTrue(retval);

            retval = GetRCBool("SomeSetting", false);
            Assert.IsTrue(retval);
        }

        [TestCase]
        [Category("Configuration")]
        public void GetRCString_Test()
        {
            StringBuilder buffer = new StringBuilder(MAX_COMMAND);
            bool retval = GetRCString("VarC", buffer, "Test", buffer.Capacity);
            string rcVal = buffer.ToString();

            Assert.IsTrue(retval);
            Assert.AreEqual("Command3", rcVal);

            retval = GetRCString("SomeSetting", buffer, "Test", buffer.Capacity);
            rcVal = buffer.ToString();

            Assert.IsFalse(retval);
            Assert.AreEqual("Test", rcVal);
        }

        [TestCase]
        [Category("Configuration")]
        public void GetRCLine_Test()
        {
            StringBuilder buffer = new StringBuilder(MAX_LINE_LENGTH);
            bool retval = GetRCLine("*Script", buffer, buffer.Capacity, null);

            Assert.IsTrue(retval);

            string[] tokens = new string[6];
            Tokenize(LCTokenize, buffer.ToString(), tokens);

            Assert.AreEqual("exec", tokens[0]);
            Assert.AreEqual("!LabelSetAlpha", tokens[1]);
            Assert.AreEqual("Initialising", tokens[2]);
            Assert.AreEqual("250", tokens[3]);
            Assert.AreEqual("3", tokens[4]);
            Assert.AreEqual("10", tokens[5]);
        }

        [TestCase]
        [Category("Configuration")]
        public void GetRCColor_Test()
        {
            var defaultVal = new LSColorRef(0);
            LSColorRef retval = GetRCColor("VarJ", defaultVal);

            Assert.AreEqual(70, retval.R);
            Assert.AreEqual(130, retval.G);
            Assert.AreEqual(180, retval.B);
            Assert.AreEqual(11829830, retval.Value);

            retval = GetRCColor("SomethingElse", defaultVal);

            Assert.AreEqual(defaultVal.Value, retval.Value);
        }

        [TestCase]
        [Category("Configuration")]
        public void GetRCCoordinate_Test()
        {
            var retval = GetRCCoordinate("VarK", 0, 0);
            Assert.AreEqual(100, retval);

            retval = GetRCCoordinate("VarL", 0, 100);
            Assert.AreEqual(20, retval);

            retval = GetRCCoordinate("SomethingElse", 50, 50);
            Assert.AreEqual(50, retval);

            retval = ParseCoordinate("1000", 50, 0);
            Assert.AreEqual(1000, retval);
        }

        [TestCase]
        [Category("Configuration")]
        public void LSGetVariable_Test()
        {
            StringBuilder value = new StringBuilder(MAX_LINE_LENGTH);
            var retval = LSGetVariable("VarJ", value);

            Assert.IsTrue(retval);
            Assert.AreEqual("4682B4", value.ToString());

            value = new StringBuilder(2);
            retval = LSGetVariable("at", value, value.Capacity);

            Assert.IsTrue(retval);
            Assert.AreEqual("@", value.ToString());
        }

        [TestCase]
        [Category("Configuration")]
        public void LSSetVariable_Test()
        {
            string val = "1234";
            LSSetVariable("Test", val);

            StringBuilder value = new StringBuilder(4);
            var retval = LSGetVariable("Test", value);

            Assert.IsTrue(retval);
            Assert.AreEqual(val, value.ToString());
        }

        [TestCase]
        [Category("Configuration")]
        public void VarExpansion_Test()
        {
            StringBuilder buffer = new StringBuilder(10);
            VarExpansion(buffer, "$VarA$");

            Assert.AreEqual("TRUE", buffer.ToString());

            buffer = new StringBuilder(MAX_COMMAND);
            VarExpansion(buffer, "$VarB$", buffer.Capacity);

            Assert.AreEqual("15", buffer.ToString());
        }

        [TestCase]
        [Category("Configuration")]
        public void GetToken_Test()
        {
            string line = "*Script exec !LabelSetAlpha Initialising 250 3 10";
            string[] tokens = line.Split(' ');

            foreach (var token in tokens)
            {
                StringBuilder buffer = new StringBuilder(token.Length);

                IntPtr ptr = Marshal.AllocHGlobal(Marshal.SizeOf(typeof(char)) * line.Length);
                IntPtr nextToken = Marshal.AllocHGlobal(Marshal.SizeOf(ptr));
                Marshal.WriteIntPtr(nextToken, ptr);

                var retval = GetToken(line, buffer, nextToken, false);

                Assert.IsTrue(retval);
                Assert.AreEqual(token, buffer.ToString());

                ptr = Marshal.ReadIntPtr(nextToken);
                line = Marshal.PtrToStringAuto(ptr);

                Marshal.FreeHGlobal(nextToken);
            }
        }

        [TestCase]
        [Category("Configuration")]
        public void Match_Test()
        {
            var retval = Match("ex?c", "exec");
            Assert.IsTrue(retval);

            retval = Match("ex*", "exec");
            Assert.IsTrue(retval);

            retval = Match("exec", "exec ");
            Assert.IsFalse(retval);

            var iretval = MatchE("[A-Z", "exec");
            Assert.AreEqual(MATCH_PATTERN, iretval);

            iretval = MatchE("exec", "exec ");
            Assert.AreEqual(MATCH_END, iretval);

            iretval = MatchE("[0-9]", "exec");
            Assert.AreEqual(MATCH_RANGE, iretval);

            iretval = MatchE("'exec", "exec");
            Assert.AreEqual(MATCH_LITERAL, iretval);

            iretval = MatchE("exe* ", "exec");
            Assert.AreEqual(MATCH_ABORT, iretval);
        }

        [TestCase]
        [Category("Configuration")]
        public void IsPatternValid_Test()
        {
            int errorCode = 0;

            var retval = IsPatternValid("exec\\", ref errorCode);

            Assert.IsFalse(retval);
            Assert.AreEqual(PATTERN_ESC, errorCode);

            retval = IsPatternValid("[]", ref errorCode);

            Assert.IsFalse(retval);
            Assert.AreEqual(PATTERN_EMPTY, errorCode);

            retval = IsPatternValid("[a-z][", ref errorCode);

            Assert.IsFalse(retval);
            Assert.AreEqual(PATTERN_CLOSE, errorCode);

            retval = IsPatternValid("[a-]", ref errorCode);

            Assert.IsFalse(retval);
            Assert.AreEqual(PATTERN_RANGE, errorCode);

            retval = IsPatternValid("[a-zA-Z_0-9]", ref errorCode);

            Assert.IsTrue(retval);
            Assert.AreEqual(PATTERN_VALID, errorCode);
        }

        [TestCase]
        [Category("Diagnostics")]
        public void LSLog_Test()
        {
            Assert.IsTrue(GetRCBool("LSLogFile", true));

            var loglevel = GetRCInt("LSLogLevel", LS_LOG_DEBUG);
            
            var retval = LSLog(loglevel, "LsapiSharpTest", "Logging test");
            Assert.IsTrue(retval);

            retval = LSLog(loglevel, "LsapiSharpTest", "Logging Test with logging level %i", loglevel);
            Assert.IsTrue(retval);
        }

        [TestCase]
        [Category("Configuration")]
        public void EnumLSData_Test()
        {
            EnumBangDelegate bangCb = (cmd, lparam) =>
            {
                LSLog(LS_LOG_DEBUG, "LsapiSharpTest", "Enum Bang command: " + cmd);
                return true;
            };

            EnumBangV2Delegate bangV2Cb = (hInst, cmd, lparam) =>
            {
                LSLog(LS_LOG_DEBUG, "LsapiSharpTest", "Enum Bang V2 command: " + cmd);
                return true;
            };

            EnumRevIdsDelegate revIdCb = (revId, lparam) =>
            {
                LSLog(LS_LOG_DEBUG, "LsapiSharpTest", "Enum RevId: " + revId);
                return true;
            };

            var retval = EnumLSData(ELD_BANGS, bangCb, IntPtr.Zero);
            Assert.AreEqual(LSHResult.S_OK, retval.Value);

            retval = EnumLSData(ELD_BANGS_V2, bangV2Cb, IntPtr.Zero);
            Assert.AreEqual(LSHResult.S_OK, retval.Value);

            retval = EnumLSData(ELD_REVIDS, revIdCb, IntPtr.Zero);
            Assert.AreEqual(LSHResult.S_OK, retval.Value);
        }

        [TestCase]
        public void GetLitestepPath_Test()
        {
            StringBuilder path = new StringBuilder(MAX_PATH);
            string lspath = Path.GetDirectoryName(stepRCPath) + @"\";

            var retval = LSGetLitestepPath(path, MAX_PATH);
            Assert.IsTrue(retval);
            Assert.AreEqual(lspath, path.ToString());
        }

        [TestCase]
        public void LSGetImagePath_Test()
        {
            StringBuilder path = new StringBuilder(MAX_PATH);
            string imgpath = Path.GetDirectoryName(stepRCPath) + @"\images\";

            var retval = LSGetImagePath(path, MAX_PATH);
            Assert.IsTrue(retval);
            Assert.AreEqual(imgpath, path.ToString());
        }

        [TestCase]
        public void Bang_Test()
        {
            const string TEST_ARGS = "testarg";

            BangCommandDelegate bang = (sender, args) => Assert.AreEqual(TEST_ARGS, args);

            BangCommandDelegate2 bang2 = (sender, cmd, args) =>
            {
                Assert.AreEqual("!Test2", cmd);
                Assert.AreEqual(TEST_ARGS, args);
            };

            var retval = AddBangCommand("!Test1", bang);
            Assert.IsTrue(retval);

            retval = AddBangCommand("!Test2", bang2);
            Assert.IsTrue(retval);

            retval = ExecuteBangCommand(IntPtr.Zero, "!Test1", TEST_ARGS);
            Assert.IsTrue(retval);

            retval = ExecuteBangCommand(IntPtr.Zero, "!Test2", TEST_ARGS);
            Assert.IsTrue(retval);

            retval = RemoveBangCommand("!Test1");
            Assert.IsTrue(retval);

            retval = RemoveBangCommand("!Test2");
            Assert.IsTrue(retval);

            retval = ExecuteBangCommand(IntPtr.Zero, "!Test1", TEST_ARGS);
            Assert.IsFalse(retval);

            retval = ExecuteBangCommand(IntPtr.Zero, "!Test2", TEST_ARGS);
            Assert.IsFalse(retval);
        }

        private static int Tokenize(
            Func<string, IntPtr, int, StringBuilder, int> tokenize,
            string line, 
            string [] tokens, 
            StringBuilder extra = null)
        {
            int intPtrSize = Marshal.SizeOf(typeof(IntPtr));
            IntPtr tokensPtr = Marshal.AllocHGlobal(intPtrSize * tokens.Length);

            for (int i = 0; i < tokens.Length; i++)
            {
                IntPtr ptr = Marshal.AllocHGlobal(Marshal.SizeOf(typeof(char)) * MAX_COMMAND);
                Marshal.WriteIntPtr(tokensPtr, i * intPtrSize, ptr);
            }

            int tokenLen = tokenize(line, tokensPtr, tokens.Length, extra);

            for (int i = 0; i < tokens.Length; i++)
            {
                IntPtr ptr = Marshal.ReadIntPtr(tokensPtr, i * intPtrSize);
                tokens[i] = Marshal.PtrToStringAuto(ptr);
                Marshal.FreeHGlobal(ptr);
            }

            Marshal.FreeHGlobal(tokensPtr);
            return tokenLen;
        }
    }
}
