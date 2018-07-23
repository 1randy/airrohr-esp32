const char TXT_CONTENT_TYPE_JSON[] PROGMEM = "application/json";
const char TXT_CONTENT_TYPE_INFLUXDB[] PROGMEM = "application/x-www-form-urlencoded";
const char TXT_CONTENT_TYPE_TEXT_HTML[] PROGMEM = "text/html; charset=utf-8";
const char TXT_CONTENT_TYPE_TEXT_PLAIN[] PROGMEM = "text/plain";
const char TXT_CONTENT_TYPE_IMAGE_SVG[] PROGMEM = "image/svg+xml";
const char TXT_CONTENT_TYPE_IMAGE_PNG[] PROGMEM = "image/png";

const char DBG_TXT_TEMPERATURE[] PROGMEM = "Temperature: ";
const char DBG_TXT_HUMIDITY[] PROGMEM = "Humidity: ";
const char DBG_TXT_PRESSURE[] PROGMEM = "Pressure: ";
const char DBG_TXT_START_READING[] PROGMEM = "Start reading ";
const char DBG_TXT_END_READING[] PROGMEM = "End reading ";

const uint8_t start_SDS_cmd[] PROGMEM = {0xAA, 0xB4, 0x06, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x06, 0xAB};
const uint8_t start_SDS_cmd_len = 19;
const uint8_t stop_SDS_cmd[] PROGMEM = {0xAA, 0xB4, 0x06, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x05, 0xAB};
const uint8_t stop_SDS_cmd_len = 19;
const uint8_t continuous_mode_SDS_cmd[] PROGMEM = {0xAA, 0xB4, 0x08, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x07, 0xAB};
const uint8_t continuous_mode_SDS_cmd_len = 19;
const uint8_t version_SDS_cmd[] PROGMEM = {0xAA, 0xB4, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x05, 0xAB};
const uint8_t version_SDS_cmd_len = 19;
const uint8_t start_PMS_cmd[] PROGMEM = { 0x42, 0x4D, 0xE4, 0x00, 0x01, 0x01, 0x74 };
const uint8_t start_PMS_cmd_len = 7;
const uint8_t stop_PMS_cmd[] PROGMEM = { 0x42, 0x4D, 0xE4, 0x00, 0x00, 0x01, 0x73 };
const uint8_t stop_PMS_cmd_len = 7;
const uint8_t continuous_mode_PMS_cmd[] PROGMEM = { 0x42, 0x4D, 0xE1, 0x00, 0x01, 0x01, 0x71 };
const uint8_t continuous_mode_PMS_cmd_len = 7;
const uint8_t start_HPM_cmd[] PROGMEM = { 0x68, 0x01, 0x01, 0x96 };
const uint8_t start_HPM_cmd_len = 4;
const uint8_t stop_HPM_cmd[] PROGMEM = { 0x68, 0x01, 0x02, 0x95 };
const uint8_t stop_HPM_cmd_len = 4;
const uint8_t continuous_mode_HPM_cmd[] PROGMEM = { 0x68, 0x01, 0x40, 0x57 };
const uint8_t continuous_mode_HPM_cmd_len = 4;

const char WEB_PAGE_HEADER[] PROGMEM = "<html>\
<head>\
<title>{t}</title>\
<meta name='viewport' content='width=device-width'>\
<style type='text/css'>\
body{font-family:Arial;margin:0}\
.content{margin:10px}\
.r{text-align:right}\
td{vertical-align:top;}\
a{text-decoration:none;padding:10px;background:#38b5ad;color:white;display:block;width:auto;border-radius:5px;}\
input[type='text']{width:100%;}\
input[type='password']{width:100%;}\
input[type='submit']{border-radius:5px;font-size:medium;padding:5px;}\
.submit_green{padding:9px !important;width:100%;border-style:none;background:#38b5ad;color:white;text-align:left;}\
.submit_red{padding:9px !important;width:100%;border-style:none;background:#d04b4b;color:white;text-align:left;}\
</style>\
</head><body>\
<div style='min-height:120px;background-color:#38b5ad;margin-bottom:20px'>\
<a href='/' style='background:none;width:0;display:inline'><img src='/images?name=luftdaten_logo' style='float:left;margin:20px'/></a>\
<h3 style='margin:0'>{tt}</h3>\
<small>ID: {id}<br/>MAC: {mac}<br/>{fwt}: {fw}</small></div><div class='content'><h4>{h} {n} {t}</h4>";

const char WEB_PAGE_FOOTER[] PROGMEM = "<br/><br/><a href='/' style='display:inline;'>{t}</a><br/><br/><br/>\
<a href='http://codefor.de/stuttgart/' target='_blank' style='display:inline;background:none;color:black;'>&copy; Open Knowledge Lab Stuttgart a.o. (Code for Germany)</a>\
</div></body></html>\r\n";

const char WEB_ROOT_PAGE_CONTENT[] PROGMEM = "<a href='/values'>{t}</a><br/>\
<a href='http://maps.luftdaten.info/' target='_blank'>{map}</a><br/>\
<a href='/config'>{conf}</a><br/>\
<a href='/removeConfig'>{conf_delete}</a><br/>\
<a href='/reset'>{restart}</a><br/><br/>\
<h4>Debug Meldungen</h4>\
<table style='width:100%;'>\
<tr><td style='width:16%;'><a href='/debug?lvl=0'>Null<br>&nbsp;</a></td>\
<td style='width:16%;'><a href='/debug?lvl=1' style='background-color:#d00000;'>Error<br>&nbsp;</a></td>\
<td style='width:16%;'><a href='/debug?lvl=2' style='background-color:#d0d000;'>Warning<br>&nbsp;</a></td>\
<td style='width:16%;'><a href='/debug?lvl=3' style='background-color:#0000d0;'>Info<br>low</a></td>\
<td style='width:16%;'><a href='/debug?lvl=4' style='background-color:#00d0d0;'>Info<br>medium</a></td>\
<td style='width:16%;'><a href='/debug?lvl=5' style='background-color:#d000d0;'>Info<br>high</a></td>\
</tr>\
</table>\
";

const char WEB_CONFIG_SCRIPT[] PROGMEM = "<script>\
function setSSID(ssid){document.getElementById('wlanssid').value=ssid.innerText||ssid.textContent;document.getElementById('wlanpwd').focus();}\
function load_wifi_list(){var x=new XMLHttpRequest();x.open('GET','/wifi');x.onload = function(){if (x.status === 200) {document.getElementById('wifilist').innerHTML = x.responseText;}};x.send();}\
</script>";

const char WEB_REMOVE_CONFIG_CONTENT[] PROGMEM = "<h3>{t}</h3>\
<table><tr><td><form method='POST' action='/removeConfig'><input type='submit' class='submit_red' name='submit' value='{b}'/></form></td><td><a href='/'>{c}</a></td></tr></table>\
";

const char WEB_RESET_CONTENT[] PROGMEM = "<h3>{t}</h3>\
<table><tr><td><form method='POST' action'/reset'><input type='submit' class='submit_red' name='submit' value='{b}'/></form></td><td><a href='/'>{c}</a></td></tr></table>\
";

// Created by http://oleddisplay.squix.ch/ Consider a donation
// In case of problems make sure that you are using the font file with the correct version!
const uint8_t Roboto_Mono_9[] PROGMEM = {
	0x07, // Width: 7
	0x0C, // Height: 12
	0x20, // First Char: 32
	0xE0, // Numbers of Chars: 224

	// Jump Table:
	0xFF, 0xFF, 0x00, 0x05,  // 32:65535
	0x00, 0x00, 0x06, 0x05,  // 33:0
	0x00, 0x06, 0x07, 0x06,  // 34:6
	0x00, 0x0D, 0x09, 0x05,  // 35:13
	0x00, 0x16, 0x0A, 0x06,  // 36:22
	0x00, 0x20, 0x0C, 0x07,  // 37:32
	0x00, 0x2C, 0x0A, 0x06,  // 38:44
	0x00, 0x36, 0x05, 0x06,  // 39:54
	0x00, 0x3B, 0x08, 0x06,  // 40:59
	0x00, 0x43, 0x08, 0x06,  // 41:67
	0x00, 0x4B, 0x09, 0x05,  // 42:75
	0x00, 0x54, 0x09, 0x05,  // 43:84
	0x00, 0x5D, 0x06, 0x06,  // 44:93
	0x00, 0x63, 0x07, 0x05,  // 45:99
	0x00, 0x6A, 0x06, 0x05,  // 46:106
	0x00, 0x70, 0x07, 0x05,  // 47:112
	0x00, 0x77, 0x09, 0x06,  // 48:119
	0x00, 0x80, 0x08, 0x06,  // 49:128
	0x00, 0x88, 0x0A, 0x06,  // 50:136
	0x00, 0x92, 0x09, 0x06,  // 51:146
	0x00, 0x9B, 0x0B, 0x06,  // 52:155
	0x00, 0xA6, 0x09, 0x06,  // 53:166
	0x00, 0xAF, 0x09, 0x06,  // 54:175
	0x00, 0xB8, 0x09, 0x05,  // 55:184
	0x00, 0xC1, 0x09, 0x06,  // 56:193
	0x00, 0xCA, 0x09, 0x06,  // 57:202
	0x00, 0xD3, 0x08, 0x05,  // 58:211
	0x00, 0xDB, 0x09, 0x06,  // 59:219
	0x00, 0xE4, 0x09, 0x05,  // 60:228
	0x00, 0xED, 0x09, 0x05,  // 61:237
	0x00, 0xF6, 0x09, 0x06,  // 62:246
	0x00, 0xFF, 0x09, 0x06,  // 63:255
	0x01, 0x08, 0x0B, 0x07,  // 64:264
	0x01, 0x13, 0x0A, 0x05,  // 65:275
	0x01, 0x1D, 0x09, 0x06,  // 66:285
	0x01, 0x26, 0x0A, 0x06,  // 67:294
	0x01, 0x30, 0x09, 0x06,  // 68:304
	0x01, 0x39, 0x0A, 0x06,  // 69:313
	0x01, 0x43, 0x09, 0x06,  // 70:323
	0x01, 0x4C, 0x0B, 0x07,  // 71:332
	0x01, 0x57, 0x0A, 0x06,  // 72:343
	0x01, 0x61, 0x0A, 0x05,  // 73:353
	0x01, 0x6B, 0x09, 0x06,  // 74:363
	0x01, 0x74, 0x0A, 0x06,  // 75:372
	0x01, 0x7E, 0x0A, 0x06,  // 76:382
	0x01, 0x88, 0x0A, 0x06,  // 77:392
	0x01, 0x92, 0x0A, 0x06,  // 78:402
	0x01, 0x9C, 0x0B, 0x07,  // 79:412
	0x01, 0xA7, 0x09, 0x06,  // 80:423
	0x01, 0xB0, 0x0C, 0x07,  // 81:432
	0x01, 0xBC, 0x0A, 0x06,  // 82:444
	0x01, 0xC6, 0x0A, 0x06,  // 83:454
	0x01, 0xD0, 0x09, 0x05,  // 84:464
	0x01, 0xD9, 0x09, 0x06,  // 85:473
	0x01, 0xE2, 0x09, 0x05,  // 86:482
	0x01, 0xEB, 0x09, 0x05,  // 87:491
	0x01, 0xF4, 0x0A, 0x05,  // 88:500
	0x01, 0xFE, 0x09, 0x05,  // 89:510
	0x02, 0x07, 0x0A, 0x06,  // 90:519
	0x02, 0x11, 0x08, 0x06,  // 91:529
	0x02, 0x19, 0x08, 0x05,  // 92:537
	0x02, 0x21, 0x08, 0x06,  // 93:545
	0x02, 0x29, 0x07, 0x05,  // 94:553
	0x02, 0x30, 0x0A, 0x05,  // 95:560
	0x02, 0x3A, 0x05, 0x05,  // 96:570
	0x02, 0x3F, 0x0A, 0x06,  // 97:575
	0x02, 0x49, 0x09, 0x06,  // 98:585
	0x02, 0x52, 0x0A, 0x06,  // 99:594
	0x02, 0x5C, 0x0A, 0x06,  // 100:604
	0x02, 0x66, 0x09, 0x06,  // 101:614
	0x02, 0x6F, 0x09, 0x06,  // 102:623
	0x02, 0x78, 0x0A, 0x06,  // 103:632
	0x02, 0x82, 0x0A, 0x06,  // 104:642
	0x02, 0x8C, 0x0C, 0x06,  // 105:652
	0x02, 0x98, 0x08, 0x06,  // 106:664
	0x02, 0xA0, 0x0A, 0x06,  // 107:672
	0x02, 0xAA, 0x0C, 0x06,  // 108:682
	0x02, 0xB6, 0x0C, 0x07,  // 109:694
	0x02, 0xC2, 0x0A, 0x06,  // 110:706
	0x02, 0xCC, 0x09, 0x06,  // 111:716
	0x02, 0xD5, 0x09, 0x06,  // 112:725
	0x02, 0xDE, 0x0A, 0x06,  // 113:734
	0x02, 0xE8, 0x07, 0x05,  // 114:744
	0x02, 0xEF, 0x0A, 0x06,  // 115:751
	0x02, 0xF9, 0x0A, 0x06,  // 116:761
	0x03, 0x03, 0x0A, 0x06,  // 117:771
	0x03, 0x0D, 0x09, 0x05,  // 118:781
	0x03, 0x16, 0x09, 0x05,  // 119:790
	0x03, 0x1F, 0x0A, 0x05,  // 120:799
	0x03, 0x29, 0x09, 0x05,  // 121:809
	0x03, 0x32, 0x0A, 0x06,  // 122:818
	0x03, 0x3C, 0x0A, 0x06,  // 123:828
	0x03, 0x46, 0x06, 0x05,  // 124:838
	0x03, 0x4C, 0x07, 0x05,  // 125:844
	0x03, 0x53, 0x09, 0x05,  // 126:851
	0xFF, 0xFF, 0x00, 0x05,  // 127:65535
	0xFF, 0xFF, 0x00, 0x05,  // 128:65535
	0xFF, 0xFF, 0x00, 0x05,  // 129:65535
	0xFF, 0xFF, 0x00, 0x05,  // 130:65535
	0xFF, 0xFF, 0x00, 0x05,  // 131:65535
	0xFF, 0xFF, 0x00, 0x05,  // 132:65535
	0xFF, 0xFF, 0x00, 0x05,  // 133:65535
	0xFF, 0xFF, 0x00, 0x05,  // 134:65535
	0xFF, 0xFF, 0x00, 0x05,  // 135:65535
	0xFF, 0xFF, 0x00, 0x05,  // 136:65535
	0xFF, 0xFF, 0x00, 0x05,  // 137:65535
	0xFF, 0xFF, 0x00, 0x05,  // 138:65535
	0xFF, 0xFF, 0x00, 0x05,  // 139:65535
	0xFF, 0xFF, 0x00, 0x05,  // 140:65535
	0xFF, 0xFF, 0x00, 0x05,  // 141:65535
	0xFF, 0xFF, 0x00, 0x05,  // 142:65535
	0xFF, 0xFF, 0x00, 0x05,  // 143:65535
	0xFF, 0xFF, 0x00, 0x05,  // 144:65535
	0xFF, 0xFF, 0x00, 0x05,  // 145:65535
	0xFF, 0xFF, 0x00, 0x05,  // 146:65535
	0xFF, 0xFF, 0x00, 0x05,  // 147:65535
	0xFF, 0xFF, 0x00, 0x05,  // 148:65535
	0xFF, 0xFF, 0x00, 0x05,  // 149:65535
	0xFF, 0xFF, 0x00, 0x05,  // 150:65535
	0xFF, 0xFF, 0x00, 0x05,  // 151:65535
	0xFF, 0xFF, 0x00, 0x05,  // 152:65535
	0xFF, 0xFF, 0x00, 0x05,  // 153:65535
	0xFF, 0xFF, 0x00, 0x05,  // 154:65535
	0xFF, 0xFF, 0x00, 0x05,  // 155:65535
	0xFF, 0xFF, 0x00, 0x05,  // 156:65535
	0xFF, 0xFF, 0x00, 0x05,  // 157:65535
	0xFF, 0xFF, 0x00, 0x05,  // 158:65535
	0xFF, 0xFF, 0x00, 0x05,  // 159:65535
	0xFF, 0xFF, 0x00, 0x05,  // 160:65535
	0x03, 0x5C, 0x06, 0x05,  // 161:860
	0x03, 0x62, 0x0A, 0x06,  // 162:866
	0x03, 0x6C, 0x0A, 0x05,  // 163:876
	0x03, 0x76, 0x0A, 0x06,  // 164:886
	0x03, 0x80, 0x09, 0x05,  // 165:896
	0x03, 0x89, 0x06, 0x05,  // 166:905
	0x03, 0x8F, 0x0A, 0x06,  // 167:911
	0x03, 0x99, 0x07, 0x05,  // 168:921
	0x03, 0xA0, 0x0B, 0x07,  // 169:928
	0x03, 0xAB, 0x07, 0x05,  // 170:939
	0x03, 0xB2, 0x07, 0x06,  // 171:946
	0x03, 0xB9, 0x09, 0x06,  // 172:953
	0x03, 0xC2, 0x07, 0x05,  // 173:962
	0x03, 0xC9, 0x0B, 0x07,  // 174:969
	0x03, 0xD4, 0x07, 0x05,  // 175:980
	0x03, 0xDB, 0x07, 0x05,  // 176:987
	0x03, 0xE2, 0x09, 0x05,  // 177:994
	0x03, 0xEB, 0x07, 0x05,  // 178:1003
	0x03, 0xF2, 0x07, 0x05,  // 179:1010
	0x03, 0xF9, 0x05, 0x05,  // 180:1017
	0x03, 0xFE, 0x0A, 0x06,  // 181:1022
	0x04, 0x08, 0x08, 0x05,  // 182:1032
	0x04, 0x10, 0x05, 0x05,  // 183:1040
	0x04, 0x15, 0x06, 0x05,  // 184:1045
	0x04, 0x1B, 0x07, 0x06,  // 185:1051
	0x04, 0x22, 0x07, 0x05,  // 186:1058
	0x04, 0x29, 0x09, 0x05,  // 187:1065
	0x04, 0x32, 0x0A, 0x06,  // 188:1074
	0x04, 0x3C, 0x0C, 0x06,  // 189:1084
	0x04, 0x48, 0x0A, 0x06,  // 190:1096
	0x04, 0x52, 0x0A, 0x06,  // 191:1106
	0x04, 0x5C, 0x0A, 0x05,  // 192:1116
	0x04, 0x66, 0x0A, 0x05,  // 193:1126
	0x04, 0x70, 0x0A, 0x05,  // 194:1136
	0x04, 0x7A, 0x0A, 0x05,  // 195:1146
	0x04, 0x84, 0x0A, 0x05,  // 196:1156
	0x04, 0x8E, 0x0A, 0x05,  // 197:1166
	0x04, 0x98, 0x0C, 0x06,  // 198:1176
	0x04, 0xA4, 0x09, 0x06,  // 199:1188
	0x04, 0xAD, 0x0A, 0x06,  // 200:1197
	0x04, 0xB7, 0x0A, 0x06,  // 201:1207
	0x04, 0xC1, 0x0A, 0x06,  // 202:1217
	0x04, 0xCB, 0x0A, 0x06,  // 203:1227
	0x04, 0xD5, 0x0A, 0x05,  // 204:1237
	0x04, 0xDF, 0x0A, 0x05,  // 205:1247
	0x04, 0xE9, 0x0A, 0x05,  // 206:1257
	0x04, 0xF3, 0x0A, 0x05,  // 207:1267
	0x04, 0xFD, 0x09, 0x06,  // 208:1277
	0x05, 0x06, 0x0A, 0x06,  // 209:1286
	0x05, 0x10, 0x0B, 0x07,  // 210:1296
	0x05, 0x1B, 0x0B, 0x07,  // 211:1307
	0x05, 0x26, 0x0B, 0x07,  // 212:1318
	0x05, 0x31, 0x0B, 0x07,  // 213:1329
	0x05, 0x3C, 0x0B, 0x07,  // 214:1340
	0x05, 0x47, 0x09, 0x05,  // 215:1351
	0x05, 0x50, 0x0B, 0x07,  // 216:1360
	0x05, 0x5B, 0x09, 0x06,  // 217:1371
	0x05, 0x64, 0x09, 0x06,  // 218:1380
	0x05, 0x6D, 0x09, 0x06,  // 219:1389
	0x05, 0x76, 0x09, 0x06,  // 220:1398
	0x05, 0x7F, 0x09, 0x05,  // 221:1407
	0x05, 0x88, 0x09, 0x06,  // 222:1416
	0x05, 0x91, 0x0A, 0x06,  // 223:1425
	0x05, 0x9B, 0x0A, 0x06,  // 224:1435
	0x05, 0xA5, 0x0A, 0x06,  // 225:1445
	0x05, 0xAF, 0x0A, 0x06,  // 226:1455
	0x05, 0xB9, 0x0A, 0x06,  // 227:1465
	0x05, 0xC3, 0x0A, 0x06,  // 228:1475
	0x05, 0xCD, 0x0A, 0x06,  // 229:1485
	0x05, 0xD7, 0x0C, 0x07,  // 230:1495
	0x05, 0xE3, 0x09, 0x06,  // 231:1507
	0x05, 0xEC, 0x09, 0x06,  // 232:1516
	0x05, 0xF5, 0x09, 0x06,  // 233:1525
	0x05, 0xFE, 0x09, 0x06,  // 234:1534
	0x06, 0x07, 0x09, 0x06,  // 235:1543
	0x06, 0x10, 0x0C, 0x06,  // 236:1552
	0x06, 0x1C, 0x0C, 0x06,  // 237:1564
	0x06, 0x28, 0x0C, 0x06,  // 238:1576
	0x06, 0x34, 0x0C, 0x06,  // 239:1588
	0x06, 0x40, 0x09, 0x06,  // 240:1600
	0x06, 0x49, 0x0A, 0x06,  // 241:1609
	0x06, 0x53, 0x09, 0x06,  // 242:1619
	0x06, 0x5C, 0x09, 0x06,  // 243:1628
	0x06, 0x65, 0x09, 0x06,  // 244:1637
	0x06, 0x6E, 0x09, 0x06,  // 245:1646
	0x06, 0x77, 0x09, 0x06,  // 246:1655
	0x06, 0x80, 0x09, 0x05,  // 247:1664
	0x06, 0x89, 0x09, 0x06,  // 248:1673
	0x06, 0x92, 0x0A, 0x06,  // 249:1682
	0x06, 0x9C, 0x0A, 0x06,  // 250:1692
	0x06, 0xA6, 0x0A, 0x06,  // 251:1702
	0x06, 0xB0, 0x0A, 0x06,  // 252:1712
	0x06, 0xBA, 0x09, 0x05,  // 253:1722
	0x06, 0xC3, 0x09, 0x06,  // 254:1731
	0x06, 0xCC, 0x09, 0x05,  // 255:1740

	// Font Data:
	0x00,0x00,0x00,0x00,0x3C,0x01,	// 33
	0x00,0x00,0x00,0x00,0x0C,0x00,0x0C,	// 34
	0x40,0x00,0xF0,0x01,0x5C,0x01,0xF8,0x00,0x54,	// 35
	0x00,0x00,0x38,0x01,0x24,0x02,0x48,0x02,0xD8,0x01,	// 36
	0x00,0x00,0x1C,0x00,0xD4,0x00,0xB8,0x00,0x48,0x01,0xC0,0x01,	// 37
	0x00,0x00,0xD8,0x01,0x24,0x01,0x5C,0x01,0xC0,0x01,	// 38
	0x00,0x00,0x00,0x00,0x0C,	// 39
	0x00,0x00,0x00,0x00,0xF0,0x01,0x0C,0x06,	// 40
	0x00,0x00,0x00,0x04,0x0C,0x02,0xF0,0x01,	// 41
	0x10,0x00,0x60,0x00,0x38,0x00,0x60,0x00,0x10,	// 42
	0x20,0x00,0x20,0x00,0xF8,0x00,0x20,0x00,0x20,	// 43
	0x00,0x00,0x00,0x00,0x00,0x03,	// 44
	0x00,0x00,0x40,0x00,0x40,0x00,0x40,	// 45
	0x00,0x00,0x00,0x00,0x00,0x01,	// 46
	0x00,0x00,0x80,0x01,0x60,0x00,0x1C,	// 47
	0x00,0x00,0xF8,0x00,0x24,0x01,0x24,0x01,0xF8,	// 48
	0x00,0x00,0x08,0x00,0x08,0x00,0xFC,0x01,	// 49
	0x00,0x00,0x88,0x01,0x44,0x01,0x24,0x01,0x18,0x01,	// 50
	0x00,0x00,0x88,0x01,0x24,0x01,0x24,0x01,0xD8,	// 51
	0x00,0x00,0xC0,0x00,0xB0,0x00,0x88,0x00,0xFC,0x01,0x80,	// 52
	0x00,0x00,0x9C,0x00,0x14,0x01,0x14,0x01,0xE4,	// 53
	0x00,0x00,0xF8,0x00,0x1C,0x01,0x14,0x01,0xE0,	// 54
	0x00,0x00,0x04,0x00,0xC4,0x01,0x34,0x00,0x0C,	// 55
	0x00,0x00,0xD8,0x00,0x24,0x01,0x24,0x01,0xD8,	// 56
	0x00,0x00,0x38,0x00,0x44,0x01,0xC4,0x01,0xF8,	// 57
	0x00,0x00,0x00,0x00,0x10,0x01,0x10,0x01,	// 58
	0x00,0x00,0x00,0x00,0x00,0x00,0x10,0x03,0x10,	// 59
	0x00,0x00,0x60,0x00,0x60,0x00,0x90,0x00,0x90,	// 60
	0x00,0x00,0x50,0x00,0x50,0x00,0x50,0x00,0x50,	// 61
	0x00,0x00,0x90,0x00,0xA0,0x00,0x60,0x00,0x40,	// 62
	0x00,0x00,0x08,0x00,0x44,0x01,0x24,0x00,0x18,	// 63
	0x00,0x00,0xF8,0x00,0xFC,0x01,0xFC,0x01,0x84,0x00,0xF8,	// 64
	0x00,0x01,0xF0,0x00,0x4C,0x00,0x78,0x00,0x80,0x01,	// 65
	0x00,0x00,0xFC,0x01,0x24,0x01,0x24,0x01,0xD8,	// 66
	0x00,0x00,0xF8,0x00,0x04,0x01,0x04,0x01,0x8C,0x01,	// 67
	0x00,0x00,0xFC,0x01,0x04,0x01,0x8C,0x01,0xF8,	// 68
	0x00,0x00,0xFC,0x01,0x24,0x01,0x24,0x01,0x24,0x01,	// 69
	0x00,0x00,0xFC,0x01,0x24,0x00,0x24,0x00,0x24,	// 70
	0x00,0x00,0xF8,0x00,0x8C,0x01,0x04,0x01,0x24,0x01,0xE8,	// 71
	0x00,0x00,0xFC,0x01,0x20,0x00,0x20,0x00,0xFC,0x01,	// 72
	0x04,0x01,0x04,0x01,0xFC,0x01,0x04,0x01,0x04,0x01,	// 73
	0x00,0x00,0x80,0x00,0x00,0x01,0x00,0x01,0xFC,	// 74
	0x00,0x00,0xFC,0x01,0x20,0x00,0x78,0x00,0x84,0x01,	// 75
	0x00,0x00,0xFC,0x01,0x00,0x01,0x00,0x01,0x00,0x01,	// 76
	0x00,0x00,0xFC,0x01,0x78,0x00,0x38,0x00,0xFC,0x01,	// 77
	0x00,0x00,0xFC,0x01,0x38,0x00,0xC0,0x00,0xFC,0x01,	// 78
	0x00,0x00,0xF8,0x00,0x0C,0x01,0x04,0x01,0x04,0x01,0xF8,	// 79
	0x00,0x00,0xFC,0x01,0x24,0x00,0x24,0x00,0x18,	// 80
	0x00,0x00,0xF8,0x00,0x0C,0x01,0x04,0x01,0x04,0x01,0xF8,0x02,	// 81
	0x00,0x00,0xFC,0x01,0x24,0x00,0xE4,0x00,0x18,0x01,	// 82
	0x00,0x00,0x98,0x00,0x24,0x01,0x24,0x01,0xCC,0x01,	// 83
	0x04,0x00,0x04,0x00,0xFC,0x01,0x04,0x00,0x04,	// 84
	0x00,0x00,0xFC,0x00,0x00,0x01,0x00,0x01,0xFC,	// 85
	0x04,0x00,0x78,0x00,0x80,0x01,0x70,0x00,0x0C,	// 86
	0x3C,0x00,0xC0,0x01,0x3C,0x00,0xF0,0x01,0xFC,	// 87
	0x00,0x00,0x8C,0x01,0x70,0x00,0xD8,0x00,0x04,0x01,	// 88
	0x04,0x00,0x18,0x00,0xE0,0x01,0x18,0x00,0x04,	// 89
	0x00,0x00,0x84,0x01,0x64,0x01,0x14,0x01,0x0C,0x01,	// 90
	0x00,0x00,0x00,0x00,0xFC,0x07,0x04,0x04,	// 91
	0x00,0x00,0x0C,0x00,0x70,0x00,0x80,0x01,	// 92
	0x00,0x00,0x00,0x00,0x04,0x04,0xFC,0x07,	// 93
	0x00,0x00,0x30,0x00,0x0C,0x00,0x18,	// 94
	0x00,0x00,0x00,0x02,0x00,0x02,0x00,0x02,0x00,0x02,	// 95
	0x00,0x00,0x00,0x00,0x04,	// 96
	0x00,0x00,0xE0,0x01,0x50,0x01,0x50,0x01,0xF0,0x01,	// 97
	0x00,0x00,0xFC,0x01,0x10,0x01,0x10,0x01,0xE0,	// 98
	0x00,0x00,0xE0,0x00,0x10,0x01,0x10,0x01,0xB0,0x01,	// 99
	0x00,0x00,0xE0,0x00,0x10,0x01,0x10,0x01,0xFC,0x01,	// 100
	0x00,0x00,0xE0,0x00,0x50,0x01,0x50,0x01,0x60,	// 101
	0x00,0x00,0x10,0x00,0xFC,0x01,0x14,0x00,0x14,	// 102
	0x00,0x00,0xE0,0x00,0x10,0x05,0x10,0x05,0xF0,0x03,	// 103
	0x00,0x00,0xFC,0x01,0x10,0x00,0x10,0x00,0xF0,0x01,	// 104
	0x00,0x00,0x10,0x01,0x10,0x01,0xF4,0x01,0x00,0x01,0x00,0x01,	// 105
	0x00,0x00,0x10,0x04,0x10,0x04,0xF4,0x03,	// 106
	0x00,0x00,0xFC,0x01,0x40,0x00,0xA0,0x00,0x10,0x01,	// 107
	0x00,0x00,0x04,0x01,0x04,0x01,0xFC,0x01,0x00,0x01,0x00,0x01,	// 108
	0x00,0x00,0xF0,0x01,0x10,0x00,0xF0,0x01,0x10,0x00,0xF0,0x01,	// 109
	0x00,0x00,0xF0,0x01,0x10,0x00,0x10,0x00,0xF0,0x01,	// 110
	0x00,0x00,0xE0,0x00,0x10,0x01,0x10,0x01,0xE0,	// 111
	0x00,0x00,0xF0,0x07,0x10,0x01,0x10,0x01,0xE0,	// 112
	0x00,0x00,0xE0,0x00,0x10,0x01,0x10,0x01,0xF0,0x07,	// 113
	0x00,0x00,0xF0,0x01,0x10,0x00,0x10,	// 114
	0x00,0x00,0xB0,0x01,0x50,0x01,0x50,0x01,0xA0,0x01,	// 115
	0x00,0x00,0x10,0x00,0xF8,0x01,0x10,0x01,0x10,0x01,	// 116
	0x00,0x00,0xF0,0x01,0x00,0x01,0x00,0x01,0xF0,0x01,	// 117
	0x10,0x00,0x60,0x00,0x80,0x01,0xE0,0x00,0x10,	// 118
	0x70,0x00,0xC0,0x01,0x30,0x00,0xC0,0x01,0xF0,	// 119
	0x00,0x00,0xB0,0x01,0x40,0x00,0xA0,0x00,0x10,0x01,	// 120
	0x10,0x00,0x60,0x04,0x80,0x03,0xE0,0x00,0x10,	// 121
	0x00,0x00,0x10,0x01,0xD0,0x01,0x30,0x01,0x10,0x01,	// 122
	0x00,0x00,0x00,0x00,0x40,0x00,0xB8,0x03,0x04,0x04,	// 123
	0x00,0x00,0x00,0x00,0xFC,0x07,	// 124
	0x00,0x00,0x04,0x04,0xB8,0x03,0x40,	// 125
	0x40,0x00,0x20,0x00,0x20,0x00,0x40,0x00,0x60,	// 126
	0x00,0x00,0x00,0x00,0x90,0x07,	// 161
	0x00,0x00,0xE0,0x01,0x10,0x01,0x08,0x02,0x10,0x01,	// 162
	0x20,0x01,0xF8,0x01,0x24,0x01,0x04,0x01,0x08,0x01,	// 163
	0x00,0x00,0xF0,0x01,0x10,0x01,0x10,0x01,0xF0,0x01,	// 164
	0xA4,0x00,0xB8,0x00,0xE0,0x01,0xB8,0x00,0xA4,	// 165
	0x00,0x00,0x00,0x00,0xDC,0x01,	// 166
	0x00,0x00,0x78,0x02,0xA4,0x04,0xA4,0x04,0xEC,0x07,	// 167
	0x00,0x00,0x04,0x00,0x00,0x00,0x04,	// 168
	0x00,0x00,0xE0,0x00,0xF0,0x01,0xB0,0x01,0x10,0x01,0xE0,	// 169
	0x00,0x00,0x78,0x00,0x54,0x00,0x7C,	// 170
	0x00,0x00,0x40,0x00,0xE0,0x00,0xA0,	// 171
	0x00,0x00,0x20,0x00,0x20,0x00,0x20,0x00,0x60,	// 172
	0x00,0x00,0x40,0x00,0x40,0x00,0x40,	// 173
	0x00,0x00,0xE0,0x00,0xF0,0x01,0x70,0x01,0x10,0x01,0xE0,	// 174
	0x00,0x00,0x04,0x00,0x04,0x00,0x04,	// 175
	0x00,0x00,0x08,0x00,0x14,0x00,0x18,	// 176
	0x20,0x01,0x20,0x01,0x78,0x01,0x20,0x01,0x20,	// 177
	0x00,0x00,0x24,0x00,0x3C,0x00,0x20,	// 178
	0x00,0x00,0x00,0x00,0x54,0x00,0x6C,	// 179
	0x00,0x00,0x00,0x00,0x04,	// 180
	0x00,0x00,0xF0,0x07,0x00,0x01,0x00,0x01,0xF0,0x01,	// 181
	0x00,0x00,0x3C,0x00,0x3C,0x00,0xFC,0x01,	// 182
	0x00,0x00,0x00,0x00,0x20,	// 183
	0x00,0x00,0x00,0x00,0x00,0x06,	// 184
	0x00,0x00,0x00,0x00,0x04,0x00,0x1C,	// 185
	0x00,0x00,0x3C,0x00,0x24,0x00,0x3C,	// 186
	0x00,0x00,0xA0,0x00,0x40,0x00,0xA0,0x00,0x40,	// 187
	0x04,0x00,0x9C,0x00,0x60,0x00,0x98,0x01,0xC0,0x01,	// 188
	0x04,0x00,0x5C,0x00,0x30,0x00,0x28,0x01,0xE0,0x01,0x00,0x01,	// 189
	0x24,0x00,0xBC,0x00,0x60,0x00,0x98,0x01,0xC0,0x01,	// 190
	0x00,0x00,0x00,0x07,0xD0,0x04,0x00,0x04,0x00,0x02,	// 191
	0x00,0x01,0xF1,0x00,0x4E,0x00,0x78,0x00,0x80,0x01,	// 192
	0x00,0x01,0xF0,0x00,0x4E,0x00,0x79,0x00,0x80,0x01,	// 193
	0x00,0x01,0xF2,0x00,0x4E,0x00,0x7A,0x00,0x80,0x01,	// 194
	0x00,0x01,0xF2,0x00,0x4D,0x00,0x7B,0x00,0x80,0x01,	// 195
	0x00,0x01,0xF2,0x00,0x4C,0x00,0x7A,0x00,0x80,0x01,	// 196
	0x00,0x01,0xF0,0x00,0x4F,0x00,0x7B,0x00,0x80,0x01,	// 197
	0x00,0x01,0xE0,0x00,0x98,0x00,0xFC,0x01,0x24,0x01,0x00,0x01,	// 198
	0x00,0x00,0xF8,0x00,0x84,0x01,0x84,0x07,0xCC,	// 199
	0x00,0x00,0xFC,0x01,0x27,0x01,0x24,0x01,0x24,0x01,	// 200
	0x00,0x00,0xFC,0x01,0x24,0x01,0x26,0x01,0x25,0x01,	// 201
	0x00,0x00,0xFC,0x01,0x26,0x01,0x26,0x01,0x24,0x01,	// 202
	0x00,0x00,0xFE,0x01,0x24,0x01,0x26,0x01,0x24,0x01,	// 203
	0x04,0x01,0x07,0x01,0xFC,0x01,0x04,0x01,0x04,0x01,	// 204
	0x04,0x01,0x04,0x01,0xFE,0x01,0x05,0x01,0x04,0x01,	// 205
	0x04,0x01,0x06,0x01,0xFD,0x01,0x06,0x01,0x04,0x01,	// 206
	0x04,0x01,0x06,0x01,0xFC,0x01,0x06,0x01,0x04,0x01,	// 207
	0x20,0x00,0xFC,0x01,0x24,0x01,0x8C,0x01,0xF8,	// 208
	0x00,0x00,0xFE,0x01,0x39,0x00,0xC3,0x00,0xFC,0x01,	// 209
	0x00,0x00,0xF8,0x00,0x0D,0x01,0x06,0x01,0x04,0x01,0xF8,	// 210
	0x00,0x00,0xF8,0x00,0x0C,0x01,0x06,0x01,0x05,0x01,0xF8,	// 211
	0x00,0x00,0xF8,0x00,0x0E,0x01,0x05,0x01,0x06,0x01,0xF8,	// 212
	0x00,0x00,0xF8,0x00,0x0F,0x01,0x05,0x01,0x07,0x01,0xF8,	// 213
	0x00,0x00,0xF8,0x00,0x0E,0x01,0x04,0x01,0x06,0x01,0xF8,	// 214
	0x00,0x00,0x90,0x00,0x60,0x00,0x60,0x00,0x90,	// 215
	0x00,0x00,0xF8,0x03,0xC4,0x01,0x24,0x01,0x1C,0x01,0xFC,	// 216
	0x00,0x00,0xFC,0x00,0x03,0x01,0x00,0x01,0xFC,	// 217
	0x00,0x00,0xFC,0x00,0x00,0x01,0x03,0x01,0xFC,	// 218
	0x00,0x00,0xFC,0x00,0x02,0x01,0x02,0x01,0xFC,	// 219
	0x00,0x00,0xFE,0x00,0x00,0x01,0x02,0x01,0xFC,	// 220
	0x04,0x00,0x18,0x00,0xE2,0x01,0x19,0x00,0x04,	// 221
	0x00,0x00,0xFC,0x01,0x90,0x00,0x90,0x00,0x60,	// 222
	0x00,0x00,0xF8,0x01,0x04,0x01,0x34,0x01,0xC8,0x01,	// 223
	0x00,0x00,0xD0,0x01,0x54,0x01,0x50,0x01,0xF0,0x01,	// 224
	0x00,0x00,0xE0,0x01,0x50,0x01,0x54,0x01,0xF0,0x01,	// 225
	0x00,0x00,0xE0,0x01,0x54,0x01,0x54,0x01,0xF0,0x01,	// 226
	0x00,0x00,0xE0,0x01,0x54,0x01,0x54,0x01,0xF0,0x01,	// 227
	0x00,0x00,0xE4,0x01,0x50,0x01,0x54,0x01,0xF0,0x01,	// 228
	0x00,0x00,0xE4,0x01,0x5A,0x01,0x54,0x01,0xF0,0x01,	// 229
	0x00,0x00,0xD0,0x01,0x50,0x01,0xE0,0x00,0x50,0x01,0x70,0x01,	// 230
	0x00,0x00,0xE0,0x00,0x90,0x01,0x90,0x07,0xA0,	// 231
	0x00,0x00,0xE0,0x00,0x54,0x01,0x50,0x01,0x60,	// 232
	0x00,0x00,0xE0,0x00,0x50,0x01,0x54,0x01,0x60,	// 233
	0x00,0x00,0xE0,0x00,0x54,0x01,0x54,0x01,0x60,	// 234
	0x00,0x00,0xE4,0x00,0x50,0x01,0x54,0x01,0x60,	// 235
	0x00,0x00,0x10,0x01,0x14,0x01,0xF0,0x01,0x00,0x01,0x00,0x01,	// 236
	0x00,0x00,0x10,0x01,0x10,0x01,0xF0,0x01,0x04,0x01,0x00,0x01,	// 237
	0x00,0x00,0x10,0x01,0x10,0x01,0xF4,0x01,0x00,0x01,0x00,0x01,	// 238
	0x00,0x00,0x10,0x01,0x14,0x01,0xF0,0x01,0x04,0x01,0x00,0x01,	// 239
	0x00,0x00,0xE0,0x00,0x18,0x01,0x1C,0x01,0xF0,	// 240
	0x00,0x00,0xF0,0x01,0x14,0x00,0x14,0x00,0xF0,0x01,	// 241
	0x00,0x00,0xE0,0x00,0x14,0x01,0x10,0x01,0xE0,	// 242
	0x00,0x00,0xE0,0x00,0x10,0x01,0x14,0x01,0xE0,	// 243
	0x00,0x00,0xE0,0x00,0x14,0x01,0x14,0x01,0xE0,	// 244
	0x00,0x00,0xE0,0x00,0x14,0x01,0x14,0x01,0xE0,	// 245
	0x00,0x00,0xE0,0x00,0x14,0x01,0x10,0x01,0xE4,	// 246
	0x20,0x00,0x20,0x00,0xA8,0x00,0x20,0x00,0x20,	// 247
	0x00,0x00,0xE0,0x00,0xD0,0x01,0x50,0x01,0xE0,	// 248
	0x00,0x00,0xF0,0x01,0x04,0x01,0x00,0x01,0xF0,0x01,	// 249
	0x00,0x00,0xF0,0x01,0x00,0x01,0x04,0x01,0xF0,0x01,	// 250
	0x00,0x00,0xF0,0x01,0x04,0x01,0x04,0x01,0xF0,0x01,	// 251
	0x00,0x00,0xF4,0x01,0x00,0x01,0x04,0x01,0xF0,0x01,	// 252
	0x10,0x00,0x60,0x04,0x80,0x03,0xE4,0x00,0x10,	// 253
	0x00,0x00,0xFC,0x07,0x10,0x01,0x10,0x01,0xE0,	// 254
	0x10,0x00,0x64,0x04,0x80,0x03,0xE4,0x00,0x10	// 255
};
