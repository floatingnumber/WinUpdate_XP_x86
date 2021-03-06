#include "stdafx.h"
#include "kb980195.h"
#include "registry.h"
#include <string>
#include <vector>

using namespace std;

bool kb980195_installed() {

	std::wstring ActiveX=L"SOFTWARE\\Microsoft\\Internet Explorer\\ActiveX Compatibility\\{";
	DWORD CompatibilityFlags;

	std::vector<std::wstring> test_keys;

	test_keys.push_back(L"00000032-9593-4264-8B29-930B3E4EDCCD");
	test_keys.push_back(L"0002E500-0000-0000-C000-000000000046");
	test_keys.push_back(L"0002E510-0000-0000-C000-000000000046");
	test_keys.push_back(L"0002E511-0000-0000-C000-000000000046");
	test_keys.push_back(L"0002E520-0000-0000-C000-000000000046");
	test_keys.push_back(L"0002E530-0000-0000-C000-000000000046");
	test_keys.push_back(L"0002E531-0000-0000-C000-000000000046");
	test_keys.push_back(L"0002E532-0000-0000-C000-000000000046");
	test_keys.push_back(L"0002E554-0000-0000-C000-000000000046");
	test_keys.push_back(L"0002E55C-0000-0000-C000-000000000046");
	test_keys.push_back(L"00D46195-B634-4C41-B53B-5093527FB791");
	test_keys.push_back(L"011B3619-FE63-4814-8A84-15A194CE9CE3");
	test_keys.push_back(L"0149EEDF-D08F-4142-8D73-D23903D21E90");
	test_keys.push_back(L"0270E604-387F-48ED-BB6D-AA51F51D6FC3");
	test_keys.push_back(L"0369B4E5-45B6-11D3-B650-00C04F79498E");
	test_keys.push_back(L"0369B4E6-45B6-11D3-B650-00C04F79498E");
	test_keys.push_back(L"038F6F55-C9F0-4601-8740-98EF1CA9DF9A");
	test_keys.push_back(L"055CB2D7-2969-45CD-914B-76890722F112");
	test_keys.push_back(L"05CDEE1D-D109-4992-B72B-6D4F5E2AB731");
	test_keys.push_back(L"0955AC62-BF2E-4CBA-A2B9-A63F772D46CF");
	test_keys.push_back(L"0B9C0C26-728C-4FDA-B8DD-59806E20E4D9");
	test_keys.push_back(L"0C378864-D5C4-4D9C-854C-432E3BEC9CCB");
	test_keys.push_back(L"0CCA191D-13A6-4E29-B746-314DEE697D83");
	test_keys.push_back(L"0ECD9B64-23AA-11D0-B351-00A0C9055D8E");
	test_keys.push_back(L"101D2283-EED9-4BA2-8F3F-23DB860946EB");
	test_keys.push_back(L"108092BF-B7DB-40D1-B7FB-F55922FCC9BE");
	test_keys.push_back(L"14C1B87C-3342-445F-9B5E-365FF330A3AC");
	test_keys.push_back(L"14FD1463-1F3F-4357-9C03-2080B442F503");
	test_keys.push_back(L"15721A53-8448-4731-8BFC-ED11E128E444");
	test_keys.push_back(L"15D6504A-5494-499C-886C-973C9E53B9F1");
	test_keys.push_back(L"17E67D4A-23A1-40D8-A049-EE34C0AF756A");
	test_keys.push_back(L"1BE49F30-0E1B-11D3-9D8E-00C04F72D980");
	test_keys.push_back(L"1C15D484-911D-11D2-B632-00C04F79498E");
	test_keys.push_back(L"1DF7D126-4050-47F0-A7CF-4C4CA9241333");
	test_keys.push_back(L"1E0D3332-7441-44FF-A225-AF48E977D8B6");
	test_keys.push_back(L"1E216240-1B7D-11CF-9D53-00AA003C9CB6");
	test_keys.push_back(L"22FD7C0A-850C-4A53-9821-0B0915C96139");
	test_keys.push_back(L"248DD896-BB45-11CF-9ABC-0080C7E7B78D");
	test_keys.push_back(L"279D6C9A-652E-4833-BEFC-312CA8887857");
	test_keys.push_back(L"27A3D328-D206-4106-8D33-1AA39B13394B");
	test_keys.push_back(L"285CAE3C-F16A-4A84-9A80-FF23D6E56D68");
	test_keys.push_back(L"2875E7A5-EE3C-4FE7-A23E-DE0529D12028");
	test_keys.push_back(L"2C2DE2E6-2AD1-4301-A6A7-DF364858EF01");
	test_keys.push_back(L"2C63E4EB-4CEA-41B8-919C-E947EA19A77C");
	test_keys.push_back(L"2D8ED06D-3C30-438B-96AE-4D110FDC1FB8");
	test_keys.push_back(L"314111B8-A502-11D2-BBCA-00C04F8EC294");
	test_keys.push_back(L"314111C6-A502-11D2-BBCA-00C04F8EC294");
	test_keys.push_back(L"3267123E-530D-4E73-9DA7-79F01D86A89F");
	test_keys.push_back(L"334125C0-77E5-11D3-B653-00C04F79498E");
	test_keys.push_back(L"3604EC19-E009-4DCB-ABC5-BB95BF92FD8B");
	test_keys.push_back(L"37B0353C-A4C8-11D2-B634-00C04F79498E");
	test_keys.push_back(L"37B03543-A4C8-11D2-B634-00C04F79498E");
	test_keys.push_back(L"37B03544-A4C8-11D2-B634-00C04F79498E");
	test_keys.push_back(L"3A2B370C-BA0A-11D1-B137-0000F8753F5D");
	test_keys.push_back(L"3BEE4890-4FE9-4A37-8C1E-5E7E12791C1F");
	test_keys.push_back(L"3D6A1A85-DE54-4768-9951-053B3B02B9B0");
	test_keys.push_back(L"40F23EB7-B397-4285-8F3C-AACE4FA40309");
	test_keys.push_back(L"41473CFB-66B6-45B8-8FB3-2BC9C1FD87BA");
	test_keys.push_back(L"418008F3-CF67-4668-9628-10DC52BE1D08");
	test_keys.push_back(L"42C68651-1700-4750-A81F-A1F5110E0F66");
	test_keys.push_back(L"44A6A9CA-AC5B-4C39-8FE6-17E7D06903A9");
	test_keys.push_back(L"4614C49A-0B7D-4E0D-A877-38CCCFE7D589");
	test_keys.push_back(L"47206204-5ECA-11D2-960F-00C04F8EE628");
	test_keys.push_back(L"4774922A-8983-4ECC-94FD-7235F06F53A1");
	test_keys.push_back(L"4788DE08-3552-49EA-AC8C-233DA52523B9");
	test_keys.push_back(L"47AF06DD-8E1B-4CA4-8F55-6B1E9FF36ACB");
	test_keys.push_back(L"497EE41C-CE06-4DD4-8308-6C730713C646");
	test_keys.push_back(L"4A5869CF-929D-4040-AE03-FCAFC5B9CD42");
	test_keys.push_back(L"4C39376E-FA9D-4349-BACC-D305C1750EF3");
	test_keys.push_back(L"4C85388F-1500-11D1-A0DF-00C04FC9E20F");
	test_keys.push_back(L"4F1E5B1A-2A80-42CA-8532-2D05CB959537");
	test_keys.push_back(L"56393399-041A-4650-94C7-13DFCB1F4665");
	test_keys.push_back(L"577FAA18-4518-445E-8F70-1473F8CF4BA4");
	test_keys.push_back(L"59DC47A8-116C-11D3-9D8E-00C04F72D980");
	test_keys.push_back(L"5C6698D9-7BE4-4122-8EC5-291D84DBD4A0");
	test_keys.push_back(L"5D80A6D1-B500-47DA-82B8-EB9875F85B4D");
	test_keys.push_back(L"5F810AFC-BB5F-4416-BE63-E01DD117BD6C");
	test_keys.push_back(L"60178279-6D62-43AF-A336-77925651A4C6");
	test_keys.push_back(L"6262D3A0-531B-11CF-91F6-C2863C385E30");
	test_keys.push_back(L"6470DE80-1635-4B5D-93A3-3701CE148A79");
	test_keys.push_back(L"648A5600-2C6E-101B-82B6-000000000014");
	test_keys.push_back(L"652623DC-2BB4-4C1C-ADFB-57A218F1A5EE");
	test_keys.push_back(L"65FB3073-CA8E-42A1-9A9A-2F826D05A843");
	test_keys.push_back(L"66E07EF9-4E89-4284-9632-6D6904B77732");
	test_keys.push_back(L"67A5F8DC-1A4B-4D66-9F24-A704AD929EEE");
	test_keys.push_back(L"68BBCA71-E1F6-47B2-87D3-369E1349D990");
	test_keys.push_back(L"692898BE-C7CC-4CB3-A45C-66508B7E2C33");
	test_keys.push_back(L"6981B978-70D9-40B9-B00E-903B6FC8CA8A");
	test_keys.push_back(L"69C462E1-CD41-49E3-9EC2-D305155718C1");
	test_keys.push_back(L"6C095616-6064-43CA-9180-CF1B6B6A0BE4");
	test_keys.push_back(L"6CA73E8B-B584-4533-A405-3D6F9C012B56");
	test_keys.push_back(L"6E5E167B-1566-4316-B27F-0DDAB3484CF7");
	test_keys.push_back(L"6F750200-1362-4815-A476-88533DE61D0C");
	test_keys.push_back(L"6F750201-1362-4815-A476-88533DE61D0C");
	test_keys.push_back(L"73BCFD0F-0DAA-4B21-B709-2A8D9D9C692A");
	test_keys.push_back(L"76EE578D-314B-4755-8365-6E1722C001A2");
	test_keys.push_back(L"784F2933-6BDD-4E5F-B1BA-A8D99B603649");
	test_keys.push_back(L"7A12547F-B772-4F2D-BE36-CE5D0FA886A1");
	test_keys.push_back(L"7EB2A2EC-1C3A-4946-9614-86D3A10EDBF3");
	test_keys.push_back(L"7F14A9EE-6989-11D5-8152-00C04F191FCA");
	test_keys.push_back(L"7F9CB14D-48E4-43B6-9346-1AEBC39C64D3");
	test_keys.push_back(L"823535A0-0318-11D3-9D8E-00C04F72D980");
	test_keys.push_back(L"833E62AD-1655-499F-908E-62DCA1EB2EC6");
	test_keys.push_back(L"86C2B477-5382-4A09-8CA3-E63B1158A377");
	test_keys.push_back(L"8872FF1B-98FA-4D7A-8D93-C9F1055F85BB");
	test_keys.push_back(L"8A674B4C-1F63-11D3-B64C-00C04F79498E");
	test_keys.push_back(L"8A674B4D-1F63-11D3-B64C-00C04F79498E");
	test_keys.push_back(L"8C7A23D9-2A9B-4AEA-BA91-3003A316B44D");
	test_keys.push_back(L"8CC18E3F-4E2B-4D27-840E-CB2F99A3A003");
	test_keys.push_back(L"8DBC7A04-B478-41D5-BE05-5545D565B59C");
	test_keys.push_back(L"8FE85D00-4647-40B9-87E4-5EB8A52F4759");
	test_keys.push_back(L"905BF7D7-6BC1-445A-BE53-9478AC096BEB");
	test_keys.push_back(L"910E7ADE-7F75-402D-A4A6-BB1A82362FCA");
	test_keys.push_back(L"916063A5-0098-4FB7-8717-1B2C62DD4E45");
	test_keys.push_back(L"926618A9-4035-4CD6-8240-64C58EB37B07");
	test_keys.push_back(L"9275A865-754B-4EDF-B828-FED0F8D344FC");
	test_keys.push_back(L"93441C07-E57E-4086-B912-F323D741A9D8");
	test_keys.push_back(L"93C5524B-97AE-491E-8EB7-2A3AD964F926");
	test_keys.push_back(L"947F2947-2296-42FE-92E6-E2E03519B895");
	test_keys.push_back(L"974E1D88-BADF-4C80-8594-A59039C992EA");
	test_keys.push_back(L"977315A5-C0DB-4EFD-89C2-10AA86CA39A5");
	test_keys.push_back(L"9BAFC7B3-F318-4BD4-BABB-6E403272615A");
	test_keys.push_back(L"9CD64701-BDF3-4D14-8E03-F12983D86664");
	test_keys.push_back(L"9E77AAC4-35E5-42A1-BDC2-8F3FF399847C");
	test_keys.push_back(L"A1A2B1C4-0E3A-11D3-9D8E-00C04F72D980");
	test_keys.push_back(L"A233E654-53FF-43AA-B1E2-60DA2E89A1EC");
	test_keys.push_back(L"A2E3074E-6C3D-11D3-B653-00C04F79498E");
	test_keys.push_back(L"A2E30750-6C3D-11D3-B653-00C04F79498E");
	test_keys.push_back(L"A3796166-A03C-418A-AF3A-060115D4E478");
	test_keys.push_back(L"A73BAEFA-EE65-494D-BEDB-DD3E5A34FA98");
	test_keys.push_back(L"A7866636-ED52-4722-82A9-6BAABEFDBF96");
	test_keys.push_back(L"A8DCF3D5-0780-4EF4-8A83-2CFFAACB8ACE");
	test_keys.push_back(L"A95845D8-8463-4605-B5FB-4F8CFBAC5C47");
	test_keys.push_back(L"A9A7297E-969C-43F1-A1EF-51EBEA36F850");
	test_keys.push_back(L"AA13BD85-7EC0-4CC8-9958-1BB2AA32FD0B");
	test_keys.push_back(L"AB049B11-607B-46C8-BBF7-F4D6AF301046");
	test_keys.push_back(L"AB237044-8A3B-42BB-9EE1-9BFA6721D9ED");
	test_keys.push_back(L"AD8E510D-217F-409B-8076-29C5E73B98E8");
	test_keys.push_back(L"AE2B937E-EA7D-4A8D-888C-B68D7F72A3C4");
	test_keys.push_back(L"AE6C4705-0F11-4ACB-BDD4-37F138BEF289");
	test_keys.push_back(L"AED98630-0251-4E83-917D-43A23D66D507");
	test_keys.push_back(L"B09DE715-87C1-11D1-8BE3-0000F8754DA1");
	test_keys.push_back(L"B0A08D67-9464-4E73-A549-2CC208AC60D3");
	test_keys.push_back(L"B0EDF163-910A-11D2-B632-00C04F79498E");
	test_keys.push_back(L"B1F78FEF-3DB7-4C56-AF2B-5DCCC7C42331");
	test_keys.push_back(L"B26E6120-DD35-4BEA-B1E3-E75F546EBF2A");
	test_keys.push_back(L"B60770C2-0390-41A8-A8DE-61889888D840");
	test_keys.push_back(L"B64016F3-C9A2-4066-96F0-BD9563314726");
	test_keys.push_back(L"B85537E9-2D9C-400A-BC92-B04F4D9FF17D");
	test_keys.push_back(L"B95B52E9-B839-4412-96EB-4DABAB2E4E24");
	test_keys.push_back(L"B9C13CD0-5A97-4C6B-8A50-7638020E2462");
	test_keys.push_back(L"BA162249-F2C5-4851-8ADC-FC58CB424243");
	test_keys.push_back(L"BB530C63-D9DF-4B49-9439-63453962E598");
	test_keys.push_back(L"BF931895-AF82-467A-8819-917C6EE2D1F3");
	test_keys.push_back(L"C05A1FBC-1413-11D1-B05F-00805F4945F6");
	test_keys.push_back(L"C3EB1670-84E0-4EDA-B570-0B51AAE81679");
	test_keys.push_back(L"C531D9FD-9685-4028-8B68-6E1232079F1E");
	test_keys.push_back(L"C5702CCC-9B79-11D3-B654-00C04F79498E");
	test_keys.push_back(L"C5702CCD-9B79-11D3-B654-00C04F79498E");
	test_keys.push_back(L"C5702CCE-9B79-11D3-B654-00C04F79498E");
	test_keys.push_back(L"C5702CCF-9B79-11D3-B654-00C04F79498E");
	test_keys.push_back(L"C5702CD0-9B79-11D3-B654-00C04F79498E");
	test_keys.push_back(L"C6B14B32-76AA-4A86-A7AC-5C79AAF58DA7");
	test_keys.push_back(L"C70D0641-DDE1-4FD7-A4D4-DA187B80741D");
	test_keys.push_back(L"C832BE8F-4B89-4579-A217-DB92E7A27915");
	test_keys.push_back(L"C86EE68A-9C77-4441-BD35-14CC6CC4A189");
	test_keys.push_back(L"C932BA85-4374-101B-A56C-00AA003668DC");
	test_keys.push_back(L"C94188F6-0F9F-46B3-8B78-D71907BD8B77");
	test_keys.push_back(L"CAAFDD83-CEFC-4E3D-BA03-175F17A24F91");
	test_keys.push_back(L"CB05A177-1069-4A7A-AB0A-5E6E00DCDB76");
	test_keys.push_back(L"CC7DA087-B7F4-4829-B038-DA01DFB5D879");
	test_keys.push_back(L"CDAF9CEC-F3EC-4B22-ABA3-9726713560F8");
	test_keys.push_back(L"CDE57A43-8B86-11D0-B3C6-00A0C90AEA82");
	test_keys.push_back(L"CF08D263-B832-42DB-8950-F40C9E672E27");
	test_keys.push_back(L"CF6866F9-B67C-4B24-9957-F91E91E788DC");
	test_keys.push_back(L"D02AAC50-027E-11D3-9D8E-00C04F72D980");
	test_keys.push_back(L"D8089245-3211-40F6-819B-9E5E92CD61A2");
	test_keys.push_back(L"D986FE4B-AE67-43C8-9A89-EADDEA3EC6B6");
	test_keys.push_back(L"DB640C86-731C-484A-AAAF-750656C9187D");
	test_keys.push_back(L"DC4F9DA0-DB05-4BB0-8FB2-03A80FE98772");
	test_keys.push_back(L"DD8C2179-1B4A-4951-B432-5DE3D1507142");
	test_keys.push_back(L"DE233AFF-8BD5-457E-B7F0-702DBEA5A828");
	test_keys.push_back(L"E0ECA9C3-D669-4EF4-8231-00724ED9288F");
	test_keys.push_back(L"E12DA4F2-BDFB-4EAD-B12F-2725251FA6B0");
	test_keys.push_back(L"E1A26BBF-26C0-401D-B82B-5C4CC67457E0");
	test_keys.push_back(L"E48BB416-C578-4A62-84C9-5E3389ABE5FC");
	test_keys.push_back(L"E4C97925-C194-4551-8831-EABBD0280885");
	test_keys.push_back(L"E6127E3B-8D17-4BEA-A039-8BB9D0D105A2");
	test_keys.push_back(L"E9CB13DB-20AB-43C5-B283-977C58FB5754");
	test_keys.push_back(L"F0E42D50-368C-11D0-AD81-00A0C90DC8D9");
	test_keys.push_back(L"F0E42D60-368C-11D0-AD81-00A0C90DC8D9");
	test_keys.push_back(L"F1F51698-7B63-4394-8743-1F4CF1853DE1");
	test_keys.push_back(L"F2175210-368C-11D0-AD81-00A0C90DC8D9");
	test_keys.push_back(L"F399F5B6-3C63-4674-B0FF-E94328B1947D");
	test_keys.push_back(L"F6A56D95-A3A3-11D2-AC26-400000058481");
	test_keys.push_back(L"F6A7FF1B-9951-4CBE-B197-EA554D6DF40D");
	test_keys.push_back(L"F89EF74A-956B-4BD3-A066-4F23DF891982");
	test_keys.push_back(L"F9769A06-7ACA-4E39-9CFB-97BB35F0E77E");
	test_keys.push_back(L"FA7C375B-66A7-4280-879D-FD459C84BB02");
	test_keys.push_back(L"FA8932FF-E064-4378-901C-69CB94E3A20A");
	test_keys.push_back(L"FA91DF8D-53AB-455D-AB20-F2F023E498D3");
	test_keys.push_back(L"FC28B75F-F9F6-4C92-AF91-14A3A51C49FB");
	test_keys.push_back(L"FFBB3F3B-0A5A-4106-BE53-DFE1E2340CB1");

	bool installed=true;
	int status;

	for(unsigned int i=0; i<test_keys.size(); i++) {
		CompatibilityFlags=regQueryDWORD(ActiveX+test_keys[i]+L"}",L"Compatibility Flags",&status);
		if(CompatibilityFlags != 1024){
			//printf("%ls: #%d#\n",test_keys[i].c_str(),CompatibilityFlags);
			installed=false;
		}
	}

	return installed;
}