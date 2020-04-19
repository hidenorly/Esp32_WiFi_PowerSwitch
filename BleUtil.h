/* 
 Copyright (C) 2020 hidenorly

 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at

      http://www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.
*/

#ifndef __BLEUTIL_H__
#define __BLEUTIL_H__

class BleUtil
{
public:
	static void saveConfig(String bleaddr);
	static void loadConfig(void);

	static String getAddr(void);

protected:
	static String mBleAddr;
};

#endif // __BLEUTIL_H__
