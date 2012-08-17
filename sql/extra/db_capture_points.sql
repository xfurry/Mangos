-- Fix capture point flags (was 48 which means locked and this shouldnt be initially)
UPDATE gameobject_template SET flags = 32 WHERE entry IN (183412, 183413, 183414, 182210);
-- Fix a capture point related gameobject flag (was 0)
UPDATE gameobject_template SET flags = 32 WHERE entry = 182529;
-- Fix animprogress, spawntimesecs of capture point gameobjects (animprogress was 100 which is TBC value, spawntimesecs was 900, 180, -300)
UPDATE gameobject SET animprogress = 255, spawntimesecs = 0 WHERE id IN (181899, 182096, 182097, 182098, 182173, 182174, 182175, 182522, 182523, 182528, 182529, 183104, 183411, 183412, 183413, 183414, 182210);


-- Silithus fixes
/* ################################# */
-- Not sure if this is actually correct
DELETE FROM spell_script_target WHERE entry=29534;
INSERT INTO spell_script_target VALUES
(29534,0,181597);
/* ################################# */

-- Venture bay fixes
/* ################################# */
DELETE FROM creature_linking_template WHERE entry IN (27758,27748);
INSERT IGNORE INTO creature_linking_template VALUES
(27758,571,27759,1,0),
(27748,571,27708,1,0);
UPDATE creature_template SET MovementType=0 WHERE entry=385;
-- Summon Venture bay npcs
DELETE FROM creature WHERE id IN (27758,27759,29252,27760,29250,27748,27708,29253,27730,29251,385,5774) AND position_z < 20 AND map=571;
-- Alliance
DELETE FROM event_scripts WHERE id IN (18035,18036);
INSERT INTO event_scripts (id, command, datalong, data_flags, x, y, z, o, comments) VALUES
-- Soldiers
(18035,10,27758,8,2515.91, -1822.01, 10.9846,  5.48033,'Alliance Venture Bay capture - summon Westfall Brigade Defender'),
(18035,10,27758,8,2494.39, -1933.88, 12.4038,  0.558505,'Alliance Venture Bay capture - summon Westfall Brigade Defender'),
(18035,10,27758,8,2525.61, -1980.78,  8.35749, 5.34071,'Alliance Venture Bay capture - summon Westfall Brigade Defender'),
(18035,10,27758,8,2539.51, -1972.8,   8.3314,  5.3058,'Alliance Venture Bay capture - summon Westfall Brigade Defender'),
(18035,10,27758,8,2462.25, -1849.97,  5.59361, 6.08343,'Alliance Venture Bay capture - summon Westfall Brigade Defender'),
(18035,10,27758,8,2496.55, -1890.7,   8.47805, 1.8787,'Alliance Venture Bay capture - summon Westfall Brigade Defender'),
(18035,10,27758,8,2552.46, -1831.56, 10.5746,  2.25148,'Alliance Venture Bay capture - summon Westfall Brigade Defender'),
(18035,10,27758,8,2574.73, -1795.42, 10.5438,  0.349066,'Alliance Venture Bay capture - summon Westfall Brigade Defender'),
(18035,10,27758,8,2579.39, -1810.58, 10.4847,  0.296706,'Alliance Venture Bay capture - summon Westfall Brigade Defender'),
(18035,10,27758,8,2481.61, -1875.83, 10.8375,  6.23082,'Alliance Venture Bay capture - summon Westfall Brigade Defender'),
(18035,10,27758,8,2481.1,  -1903.97, 10.5534,  6.24828,'Alliance Venture Bay capture - summon Westfall Brigade Defender'),
(18035,10,27758,8,2543.5,  -1915.32,  3.39682, 0.034907,'Alliance Venture Bay capture - summon Westfall Brigade Defender'),
(18035,10,27758,8,2544.33, -1930.5,   3.86072, 0,'Alliance Venture Bay capture - summon Westfall Brigade Defender'),
(18035,10,27759,8,2484.83, -1888.76,  9.76869, 0.05236,'Alliance Venture Bay capture - summon Commander Howser'),
-- Vendors
(18035,10,29252,8,2476.89, -1953.83, 10.9665, 1.44862,'Alliance Venture Bay capture - summon Jason Riggins'),
(18035,10,27760,8,2492.09, -1839.39, 11.752,  5.58505,'Alliance Venture Bay capture - summon "Grizzly" D. Adams'),
(18035,10,29250,8,2551.9,  -1836.04, 10.637,  2.11185,'Alliance Venture Bay capture - summon Tim Street'),
-- Horses
(18035,10,385,8,2552.79, -1840.45, 10.6082, 5.34071,'Alliance Venture Bay capture - summon Horse'),
(18035,10,385,8,2560.17, -1834.67, 10.6368, 5.28835,'Alliance Venture Bay capture - summon Horse'),
(18035,10,385,8,2556.78, -1846.09, 10.2806, 2.21657,'Alliance Venture Bay capture - summon Horse'),
(18035,10,385,8,2563.99, -1840.05, 10.6088, 2.16421,'Alliance Venture Bay capture - summon Horse');
-- Horde
INSERT INTO event_scripts (id, command, datalong, data_flags, x, y, z, o, comments) VALUES
-- Soldiers
(18036,10,27748,8,2538.89, -1922.25,  3.143,   0.123386,'Horde Venture Bay capture - summon Conquest Hold Defender'),
(18036,10,27748,8,2570.12, -1805.95, 10.0925,  0.453786,'Horde Venture Bay capture - summon Conquest Hold Defender'),
(18036,10,27748,8,2553.65, -1832.93, 10.6207,  2.25148,'Horde Venture Bay capture - summon Conquest Hold Defender'),
(18036,10,27748,8,2409.69, -1825.34,  3.72895, 2.67035,'Horde Venture Bay capture - summon Conquest Hold Defender'),
(18036,10,27748,8,2578.04, -1809.41, 10.3817,  0.314159,'Horde Venture Bay capture - summon Conquest Hold Defender'),
(18036,10,27748,8,2574.45, -1797.03, 10.4851,  0.331613,'Horde Venture Bay capture - summon Conquest Hold Defender'),
(18036,10,27748,8,2477.94, -1875.99, 10.5229,  0.034907,'Horde Venture Bay capture - summon Conquest Hold Defender'),
(18036,10,27748,8,2500.57, -1835.32, 10.6343,  5.58505,'Horde Venture Bay capture - summon Conquest Hold Defender'),
(18036,10,27748,8,2478.88, -1904.98, 10.4264,  0.436332,'Horde Venture Bay capture - summon Conquest Hold Defender'),
(18036,10,27748,8,2489.79, -1847.66, 10.6083,  5.65487,'Horde Venture Bay capture - summon Conquest Hold Defender'),
(18036,10,27748,8,2546.33, -1929.54,  3.42336, 0.034907,'Horde Venture Bay capture - summon Conquest Hold Defender'),
(18036,10,27748,8,2546.15, -1914.97,  3.16499, 0, 'Horde Venture Bay capture - summon Conquest Hold Defender'),
(18036,10,27748,8,2525.53, -1979.8,   8.35698, 5.25344, 'Horde Venture Bay capture - summon Conquest Hold Defender'),
(18036,10,27748,8,2538.85, -1972.3,   8.29618, 5.3058, 'Horde Venture Bay capture - summon Conquest Hold Defender'),
(18036,10,27708,8,2487.56, -1888.8,   9.71184, 0.017453, 'Horde Venture Bay capture - summon General Gorlok'),
-- Vendors
(18036,10,29253,8,2476.03, -1954, 10.9665, 1.29154,'Horde Venture Bay capture - summon Koloth'),
(18036,10,27730,8,2489.92, -1821.4, 11.7098, 5.51804,'Horde Venture Bay capture - summon Purkom'),
(18036,10,29251,8,2551.26, -1837.19, 10.6369, 2.1293,'Horde Venture Bay capture - summon Kor'),
-- Wolfs
(18036,10,5774,8,2560.6, -1835.13, 10.6364, 5.32325,'Horde Venture Bay capture - summon Riding Wolf'),
(18036,10,5774,8,2552.92,-1840.63, 10.604,  5.28835,'Horde Venture Bay capture - summon Riding Wolf'),
(18036,10,5774,8,2556.9, -1845.99, 10.2894, 2.23402,'Horde Venture Bay capture - summon Riding Wolf'),
(18036,10,5774,8,2563.97,-1840.5,  10.602,  2.14675,'Horde Venture Bay capture - summon Riding Wolf');
/* ################################# */


-- Zangarmarsh fixes
/* ################################# */
-- UPDATE `creature` SET `id` = 18757, `position_x` = 340.453, `position_y` = 6833.1, `position_z` = 61.798 WHERE `guid` = 67037;
-- UPDATE `creature` SET `MovementType` = 0, `spawndist` = 0 WHERE `id` IN (18757,18759);
-- PvP Beams
DELETE FROM creature WHERE guid IN (67037,67038,67039,84756);
DELETE FROM creature_template_addon WHERE entry IN (18759);
UPDATE creature_template SET InhabitType=InhabitType|4 WHERE entry IN (18757,18759);
-- despawn horde graveyard flag by default
UPDATE gameobject SET spawnTimeSecs = -300 WHERE id = 182528;

-- Zangarmarsh Field Scout gossips - TODO: conditions, horde gossip 1 text is guessed, horde text_id 2 is missing
UPDATE creature_template SET npcFlag=npcFlag|1 WHERE entry IN (18581,18564);
-- Alliance Field Scout
UPDATE creature_template SET gossip_menu_id = 7724 WHERE entry = 18581;
DELETE FROM gossip_menu WHERE entry = 7724;
INSERT INTO gossip_menu (entry, text_id) VALUES
(7724, 9433),
(7724, 9432);
DELETE FROM gossip_menu_option WHERE menu_id = 7724;
INSERT INTO gossip_menu_option (menu_id, id, option_icon, option_text, box_coded, box_money, box_text, option_id, npc_option_npcflag, action_script_id) VALUES
(7724, 0, 0, 'Give me a battle standard. I will take control of Twin Spire Ruins.', 0, 0, '', 1, 1, 7724),
(7724, 1, 1, 'I have marks to redeem!', 0, 0, '', 3, 128, 0);
DELETE FROM gossip_scripts WHERE id = 7724;
INSERT INTO gossip_scripts (id, delay, command, datalong, datalong2, comments)
VALUES
(7724, 0, 15, 32430, 0, 'Cast Battle Standard - Alliance');
-- Horde Field Scout
UPDATE creature_template SET gossip_menu_id = 7722 WHERE entry = 18564;
DELETE FROM gossip_menu WHERE entry = 7722;
INSERT INTO gossip_menu (entry, text_id) VALUES
(7722, 9431);
-- << missing second text_id
DELETE FROM gossip_menu_option WHERE menu_id = 7722;
INSERT INTO gossip_menu_option (menu_id, id, option_icon, option_text, box_coded, box_money, box_text, option_id, npc_option_npcflag, action_script_id) VALUES
(7722, 0, 0, 'Give me a battle standard. I will take control of Twin Spire Ruins.', 0, 0, '', 1, 1, 7722), -- << text guessed
(7722, 1, 1, 'I have marks to redeem!', 0, 0, '', 3, 128, 0);
DELETE FROM gossip_scripts WHERE id = 7722;
INSERT INTO gossip_scripts (id, delay, command, datalong, datalong2, comments)
VALUES
(7722, 0, 15, 32431, 0, 'Cast Battle Standard - Horde');
/* ################################# */


-- Eastern Plaguelands fixes
/* ################################# */
-- correct horde gameobject faction
UPDATE gameobject_template SET faction=1314 WHERE entry=181955;
-- spectral flight master aura removed as it depends on faction and is done by opvp script
DELETE FROM creature_template_addon WHERE entry=17209;
-- spectral flight master gossip scripts
UPDATE gossip_menu_option SET action_script_id=737901 WHERE menu_id=7379 AND id=0;
UPDATE gossip_menu_option SET action_script_id=737902 WHERE menu_id=7379 AND id=1;
UPDATE gossip_menu_option SET action_script_id=737903 WHERE menu_id=7379 AND id=2;
DELETE FROM gossip_scripts WHERE id IN (737901,737902,737903);
INSERT INTO gossip_scripts (id,command,datalong,comments) VALUES
(737901,30,494,'William Kielar - Send Northpass Tower taxi'),
(737902,30,495,'William Kielar - Send Eastwall Tower taxi'),
(737903,30,496,'William Kielar - Send Crown Guard Tower taxi');

-- Summon plaguewood flight master
DELETE FROM creature WHERE id = 17209;
DELETE FROM event_scripts WHERE id IN (10701,10700);
INSERT INTO event_scripts (id, command, datalong, data_flags, x, y, z, o, comments) VALUES
(10701,10,17209,8,2987.5,-3049.11,120.126,5.75959,'Alliance Plaguewood Tower progress event - summon William Kielar'),
(10700,10,17209,8,2987.5,-3049.11,120.126,5.75959,'Horde Plaguewood Tower progress event - summon William Kielar');
-- Summon eastwall soldiers
DELETE FROM creature WHERE id IN (17635,17995);
DELETE FROM event_scripts WHERE id IN (10691,10692);
INSERT INTO event_scripts (id, command, datalong, data_flags, x, y, z, o, comments) VALUES
-- alliance
(10691,10,17635,8,2526.220,-4758.520,101.056,2.17,'Alliance Eastwall Tower capture - summon Lordaeron Commander'),
(10691,10,17647,8,2532.452,-4760.138,102.408,2.17,'Alliance Eastwall Tower capture - summon Lordaeron Soldier'),
(10691,10,17647,8,2535.058,-4757.152,102.219,2.17,'Alliance Eastwall Tower capture - summon Lordaeron Soldier'),
(10691,10,17647,8,2526.297,-4764.442,102.360,2.17,'Alliance Eastwall Tower capture - summon Lordaeron Soldier'),
(10691,10,17647,8,2522.425,-4767.049,102.552,2.17,'Alliance Eastwall Tower capture - summon Lordaeron Soldier'),
-- horde
(10692,10,17995,8,2526.220,-4758.520,101.056,2.17,'Horde Eastwall Tower capture - summon Lordaeron Veteran'),
(10692,10,17996,8,2532.452,-4760.138,102.408,2.17,'Horde Eastwall Tower capture - summon Lordaeron Fighter'),
(10692,10,17996,8,2535.058,-4757.152,102.219,2.17,'Horde Eastwall Tower capture - summon Lordaeron Fighter'),
(10692,10,17996,8,2526.297,-4764.442,102.360,2.17,'Horde Eastwall Tower capture - summon Lordaeron Fighter'),
(10692,10,17996,8,2522.425,-4767.049,102.552,2.17,'Horde Eastwall Tower capture - summon Lordaeron Fighter');

-- set soldiers movement - TODO: needs more adjustments and testing
UPDATE creature_template SET MovementType=0 WHERE entry IN (17647,17996);
UPDATE creature_template SET MovementType=2 WHERE entry in (17635,17995);
DELETE FROM creature_movement_template WHERE entry in (17635, 17995);
INSERT INTO creature_movement_template (entry, point, position_x, position_y, position_z, script_id) VALUES
-- alliance
(17635, 1, 2501.349, -4725.994, 90.974, 0),
(17635, 2, 2491.211, -4693.162, 82.363, 0),
(17635, 3, 2493.059, -4655.492, 75.194, 0),
(17635, 4, 2562.227, -4646.404, 79.003, 0),
(17635, 5, 2699.748, -4567.377, 87.460, 0),
(17635, 6, 2757.274, -4527.591, 89.080, 0),
(17635, 7, 2850.868, -4417.565, 89.421, 0),
(17635, 8, 2888.340, -4328.486, 90.562, 0),
(17635, 9, 2913.271, -4167.140, 93.919, 0),
(17635, 10, 3035.656, -4260.176, 96.141, 0),
(17635, 11, 3088.544, -4250.208, 97.769, 0),
(17635, 12, 3147.302, -4318.796, 130.410, 0),
(17635, 13, 3166.813, -4349.198, 137.569, 1763501),
-- horde
(17995, 1, 2501.349, -4725.994, 90.974, 0),
(17995, 2, 2491.211, -4693.162, 82.363, 0),
(17995, 3, 2493.059, -4655.492, 75.194, 0),
(17995, 4, 2562.227, -4646.404, 79.003, 0),
(17995, 5, 2699.748, -4567.377, 87.460, 0),
(17995, 6, 2757.274, -4527.591, 89.080, 0),
(17995, 7, 2850.868, -4417.565, 89.421, 0),
(17995, 8, 2888.340, -4328.486, 90.562, 0),
(17995, 9, 2913.271, -4167.140, 93.919, 0),
(17995, 10, 3035.656, -4260.176, 96.141, 0),
(17995, 11, 3088.544, -4250.208, 97.769, 0),
(17995, 12, 3147.302, -4318.796, 130.410, 0),
(17995, 13, 3166.813, -4349.198, 137.569, 1799501);

DELETE FROM creature_movement_scripts WHERE id IN (1763501,1799501);
INSERT INTO creature_movement_scripts (id, command, datalong, comments) VALUES
(1763501,20,0,'Lordaeron Commander - set movement to idle'),
(1799501,20,0,'Lordaeron Veteran - set movement to idle');

-- creature linking for EP soldiers
DELETE FROM creature_linking_template WHERE entry IN (17647,17996);
INSERT INTO creature_linking_template VALUES
(17647, 0, 17635, 515, 0),
(17996, 0, 17995, 515, 0);
/* ################################# */

-- Halaa fixes
/* ################################# */
-- fire bomb target
UPDATE creature_template SET AIName='EventAI' WHERE entry=18225;
DELETE FROM creature_ai_scripts WHERE creature_id=18225;
INSERT INTO creature_ai_scripts VALUES 
('1822501','18225','11','0','100','0','0','0','0','0','11','31961','0','0','0','0','0','0','0','0','0','0','Fire Bomb Target - Cast Fire Bomb on Spawned');
-- No random movement for the fire bomb target - unit flags are guesswork
UPDATE creature_template SET MovementType= 0, unit_flags=unit_flags|33554432 WHERE entry=18225;

UPDATE creature_template SET MovementType= 0 WHERE entry IN (18817,18822,21485,21487,21488,18256);
UPDATE creature_template SET MovementType= 0 WHERE entry IN (18816,18821,21474,21484,21483,18192);
-- summon npcs by script
DELETE FROM creature WHERE id IN (18817,18822,21485,21487,21488,18256,18816,18821,21474,21484,21483,18192);
DELETE FROM event_scripts WHERE id IN (11504,11503);
INSERT INTO event_scripts (id, command, datalong, data_flags, x, y, z, o, comments) VALUES
-- alliance vendors
(11504,10,18817,8, -1591.18, 8020.39, -22.2042, 4.59022,'Alliance Halaa capture - summon Chief Researcher Kartos'),
(11504,10,18822,8, -1588.12, 8019.44, -22.2042, 4.06662,'Alliance Halaa capture - summon Quartermaster Davian Vaclav'),
(11504,10,21485,8, -1521.93, 7927.37, -20.2299, 3.24631,'Alliance Halaa capture - summon Aldraan'),
(11504,10,21487,8, -1540.33, 7971.95, -20.7186, 3.07178,'Alliance Halaa capture - summon Cendrii'),
(11504,10,21488,8, -1570.01, 7993.8, -22.4505, 5.02655,'Alliance Halaa capture - summon Banro'),
-- soldiers
(11504,10,18256,8, -1654.06, 8000.46, -26.59, 3.37, 'Alliance Halaa capture - summon Alliance Halaani Guard'),
(11504,10,18256,8, -1487.18, 7899.1, -19.53, 0.954, 'Alliance Halaa capture - summon Alliance Halaani Guard'),
(11504,10,18256,8, -1480.88, 7908.79, -19.19, 4.485, 'Alliance Halaa capture - summon Alliance Halaani Guard'),
(11504,10,18256,8, -1540.56, 7995.44, -20.45, 0.947, 'Alliance Halaa capture - summon Alliance Halaani Guard'),
(11504,10,18256,8, -1546.95, 8000.85, -20.72, 6.035, 'Alliance Halaa capture - summon Alliance Halaani Guard'),
(11504,10,18256,8, -1595.31, 7860.53, -21.51, 3.747, 'Alliance Halaa capture - summon Alliance Halaani Guard'),
(11504,10,18256,8, -1642.31, 7995.59, -25.8, 3.317, 'Alliance Halaa capture - summon Alliance Halaani Guard'),
(11504,10,18256,8, -1545.46, 7995.35, -20.63, 1.094, 'Alliance Halaa capture - summon Alliance Halaani Guard'),
(11504,10,18256,8, -1487.58, 7907.99, -19.27, 5.567, 'Alliance Halaa capture - summon Alliance Halaani Guard'),
(11504,10,18256,8, -1651.54, 7988.56, -26.52, 2.984, 'Alliance Halaa capture - summon Alliance Halaani Guard'),
(11504,10,18256,8, -1602.46, 7866.43, -22.11, 4.747, 'Alliance Halaa capture - summon Alliance Halaani Guard'),
(11504,10,18256,8, -1591.22, 7875.29, -22.35, 4.345, 'Alliance Halaa capture - summon Alliance Halaani Guard'),
(11504,10,18256,8, -1603.75, 8000.36, -24.18, 4.516, 'Alliance Halaa capture - summon Alliance Halaani Guard'),
(11504,10,18256,8, -1585.73, 7994.68, -23.29, 4.439, 'Alliance Halaa capture - summon Alliance Halaani Guard'),
(11504,10,18256,8, -1595.5, 7991.27, -23.53, 4.738, 'Alliance Halaa capture - summon Alliance Halaani Guard');
INSERT INTO event_scripts (id, command, datalong, data_flags, x, y, z, o, comments) VALUES
-- horde vendors
(11503,10,18816,8, -1523.92, 7951.76, -17.6942, 3.51172, 'Horde Halaa capture - summon Chief Researcher Amereldine'),
(11503,10,18821,8, -1527.75, 7952.46, -17.6948, 3.99317, 'Horde Halaa capture - summon Quartermaster Jaffrey Noreliqe'),
(11503,10,21474,8, -1520.14, 7927.11, -20.2527, 3.39389, 'Horde Halaa capture - summon Coreiel'),
(11503,10,21484,8, -1524.84, 7930.34, -20.182, 3.6405, 'Horde Halaa capture - summon Embelar'),
(11503,10,21483,8, -1570.01, 7993.8, -22.4505, 5.02655, 'Horde Halaa capture - summon Tasaldan'),
-- soldiers
(11503,10,18192,8, -1654.06, 8000.46, -26.59, 3.37, 'Horde Halaa capture - summon Horde Halaani Guard'),
(11503,10,18192,8, -1487.18, 7899.1, -19.53, 0.954, 'Horde Halaa capture - summon Horde Halaani Guard'),
(11503,10,18192,8, -1480.88, 7908.79, -19.19, 4.485, 'Horde Halaa capture - summon Horde Halaani Guard'),
(11503,10,18192,8, -1540.56, 7995.44, -20.45, 0.947, 'Horde Halaa capture - summon Horde Halaani Guard'),
(11503,10,18192,8, -1546.95, 8000.85, -20.72, 6.035, 'Horde Halaa capture - summon Horde Halaani Guard'),
(11503,10,18192,8, -1595.31, 7860.53, -21.51, 3.747, 'Horde Halaa capture - summon Horde Halaani Guard'),
(11503,10,18192,8, -1642.31, 7995.59, -25.8, 3.317, 'Horde Halaa capture - summon Horde Halaani Guard'),
(11503,10,18192,8, -1545.46, 7995.35, -20.63, 1.094, 'Horde Halaa capture - summon Horde Halaani Guard'),
(11503,10,18192,8, -1487.58, 7907.99, -19.27, 5.567, 'Horde Halaa capture - summon Horde Halaani Guard'),
(11503,10,18192,8, -1651.54, 7988.56, -26.52, 2.984, 'Horde Halaa capture - summon Horde Halaani Guard'),
(11503,10,18192,8, -1602.46, 7866.43, -22.11, 4.747, 'Horde Halaa capture - summon Horde Halaani Guard'),
(11503,10,18192,8, -1591.22, 7875.29, -22.35, 4.345, 'Horde Halaa capture - summon Horde Halaani Guard'),
(11503,10,18192,8, -1550.6, 7944.45, -21.63, 3.559, 'Horde Halaa capture - summon Horde Halaani Guard'),
(11503,10,18192,8, -1545.57, 7935.83, -21.13, 3.448, 'Horde Halaa capture - summon Horde Halaani Guard'),
(11503,10,18192,8, -1550.86, 7937.56, -21.7, 3.801, 'Horde Halaa capture - summon Horde Halaani Guard');

-- Gameobject scripts
-- South (ally & horde)
-- Script id: 182267
DELETE FROM gameobject_template_scripts WHERE id=182267;
INSERT INTO gameobject_template_scripts VALUES
(182267,0,17,24538,10,0,0,0,0,0,0,0,0,0,0,0,'Add 10 Fire Bombs to inventory'),
(182267,0,30,520,0,0,0,0,0,0,0,0,0,0,0,0,'Send Taxi path 520');
-- Script id: 182301
DELETE FROM gameobject_template_scripts WHERE id=182301;
INSERT INTO gameobject_template_scripts VALUES
(182301,0,17,24538,10,0,0,0,0,0,0,0,0,0,0,0,'Add 10 Fire Bombs to inventory'),
(182301,0,30,520,0,0,0,0,0,0,0,0,0,0,0,0,'Send Taxi path 520');

-- West (ally & horde)
-- Script id: 182280
DELETE FROM gameobject_template_scripts WHERE id=182280;
INSERT INTO gameobject_template_scripts VALUES
(182280,0,17,24538,10,0,0,0,0,0,0,0,0,0,0,0,'Add 10 Fire Bombs to inventory'),
(182280,0,30,523,0,0,0,0,0,0,0,0,0,0,0,0,'Send Taxi path 523');
-- Script id: 182302
DELETE FROM gameobject_template_scripts WHERE id=182302;
INSERT INTO gameobject_template_scripts VALUES
(182302,0,17,24538,10,0,0,0,0,0,0,0,0,0,0,0,'Add 10 Fire Bombs to inventory'),
(182302,0,30,523,0,0,0,0,0,0,0,0,0,0,0,0,'Send Taxi path 523');

-- North (ally & horde)
-- Script id: 182281
DELETE FROM gameobject_template_scripts WHERE id=182281;
INSERT INTO gameobject_template_scripts VALUES
(182281,0,17,24538,10,0,0,0,0,0,0,0,0,0,0,0,'Add 10 Fire Bombs to inventory'),
(182281,0,30,522,0,0,0,0,0,0,0,0,0,0,0,0,'Send Taxi path 522');
-- Script id: 182303
DELETE FROM gameobject_template_scripts WHERE id=182303;
INSERT INTO gameobject_template_scripts VALUES
(182303,0,17,24538,10,0,0,0,0,0,0,0,0,0,0,0,'Add 10 Fire Bombs to inventory'),
(182303,0,30,522,0,0,0,0,0,0,0,0,0,0,0,0,'Send Taxi path 522');

-- East (ally & horde)
-- Script id: 182282
DELETE FROM gameobject_template_scripts WHERE id=182282;
INSERT INTO gameobject_template_scripts VALUES
(182282,0,17,24538,10,0,0,0,0,0,0,0,0,0,0,0,'Add 10 Fire Bombs to inventory'),
(182282,0,30,524,0,0,0,0,0,0,0,0,0,0,0,0,'Send Taxi path 524');
-- Script id: 182304
DELETE FROM gameobject_template_scripts WHERE id=182304;
INSERT INTO gameobject_template_scripts VALUES
(182304,0,17,24538,10,0,0,0,0,0,0,0,0,0,0,0,'Add 10 Fire Bombs to inventory'),
(182304,0,30,524,0,0,0,0,0,0,0,0,0,0,0,0,'Send Taxi path 524');

/* ################################# */
