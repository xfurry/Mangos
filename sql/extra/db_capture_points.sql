-- Fix capture point flags (was 48 which means locked and this shouldnt be initially)
UPDATE gameobject_template SET flags = 32 WHERE entry IN (183412, 183413, 183414, 182210);
-- Fix a capture point related gameobject flag (was 0)
UPDATE gameobject_template SET flags = 32 WHERE entry = 182529;
-- Fix animprogress, spawntimesecs of capture point gameobjects (animprogress was 100 which is TBC value, spawntimesecs was 900, 180, -300)
UPDATE gameobject SET animprogress = 255, spawntimesecs = 0 WHERE id IN (181899, 182096, 182097, 182098, 182173, 182174, 182175, 182522, 182523, 182528, 182529, 183104, 183411, 183412, 183413, 183414, 182210);


-- Silithus fixes
/* ################################# */
-- Not sure if this is actually correct
delete from spell_script_target where entry=29534;
insert into spell_script_target values
(29534,0,181597);
/* ################################# */

-- Venture bay fixes
/* ################################# */
-- creatures for world pvp - use guid from 300xxx
DELETE FROM `creature` WHERE `id` IN (27730,27760);
INSERT INTO `creature` (`guid`,`id`,`map`,`spawnMask`,`phaseMask`,`modelid`,`equipment_id`,`position_x`,`position_y`,`position_z`,`orientation`,`spawntimesecs`,`spawndist`,`currentwaypoint`,`curhealth`,`curmana`,`DeathState`,`MovementType`) VALUES
(300001, 27730, 571, 1, 1, 0, 0, 2489.92, -1821.4, 11.7098, 5.51804, 604800, 0, 0, 10282, 0, 0, 0),
(300002, 27760, 571, 1, 1, 0, 0, 2483.68, -1836.49, 11.8173, 0.734973, 604800, 0, 0, 20564, 0, 0, 0);
/* ################################# */


-- Zangarmarsh fixes
/* ################################# */
UPDATE `creature` SET `id` = 18757, `position_x` = 340.453, `position_y` = 6833.1, `position_z` = 61.798 WHERE `guid` = 67037;
UPDATE `creature` SET `MovementType` = 0, `spawndist` = 0 WHERE `id` IN (18757,18759);
DELETE FROM `creature_template_addon` WHERE `entry` IN (18757,18759);
/* ################################# */


-- Zangarmarsh Field Scout gossips - TODO: conditions, horde gossip 1 text is guessed, horde text_id 2 is missing
/* ################################# */
-- PvP Beams
DELETE FROM creature WHERE guid IN (67037,67038,67039,84756);
DELETE FROM creature_template_addon WHERE entry IN (18759);
UPDATE creature_template SET InhabitType=InhabitType|4 WHERE entry IN (18757,18759);
-- Alliance Field Scout
UPDATE creature_template SET gossip_menu_id = 7724 WHERE entry = 18581;
DELETE FROM gossip_menu WHERE entry = 7724;
INSERT INTO gossip_menu (entry, text_id) VALUES
(7724, 9433),
(7724, 9432);
DELETE FROM gossip_menu_option WHERE menu_id = 7724;
INSERT INTO gossip_menu_option (menu_id, id, option_icon, option_text, box_coded, box_money, box_text, option_id, npc_option_npcflag, action_script_id) VALUES
(7224, 0, 0, 'Give me a battle standard. I will take control of Twin Spire Ruins.', 0, 0, '', 1, 1, 7724),
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
-- correct horde faction
UPDATE `gameobject_template` SET `faction` = 1314 WHERE `entry` = 181955;

-- Summon plaguewood flightmaster
DELETE FROM event_scripts WHERE id IN (10701,10700);
INSERT INTO event_scripts (id, command, datalong, data_flags, x, y, z, o, comments) VALUES
(10701,10,17209,8,2987.5,-3049.11,120.126,5.75959,'Alliance Plaguewood Tower progress event - summon William Kielar'),
(10700,10,17209,8,2987.5,-3049.11,120.126,5.75959,'Horde Plaguewood Tower progress event - summon William Kielar');
-- Summon eastwall soldiers
DELETE FROM event_scripts WHERE id IN (10691,10692);
INSERT INTO event_scripts (id, command, datalong, data_flags, x, y, z, o, comments) VALUES
(10691,10,17635,8,2526.220,-4758.520,101.056,0,'Alliance Eastwall Tower capture - summon Lordaeron Commander'),
(10691,10,17647,8,2532.452,-4760.138,102.408,0,'Alliance Eastwall Tower capture - summon Lordaeron Soldier'),
(10691,10,17647,8,2535.058,-4757.152,102.219,0,'Alliance Eastwall Tower capture - summon Lordaeron Soldier'),
(10691,10,17647,8,2526.297,-4764.442,102.360,0,'Alliance Eastwall Tower capture - summon Lordaeron Soldier'),
(10691,10,17647,8,2522.425,-4767.049,102.552,0,'Alliance Eastwall Tower capture - summon Lordaeron Soldier'),
(10692,10,17995,8,2526.220,-4758.520,101.056,0,'Horde Eastwall Tower capture - summon Lordaeron Veteran'),
(10692,10,17996,8,2532.452,-4760.138,102.408,0,'Horde Eastwall Tower capture - summon Lordaeron Fighter'),
(10692,10,17996,8,2535.058,-4757.152,102.219,0,'Horde Eastwall Tower capture - summon Lordaeron Fighter'),
(10692,10,17996,8,2526.297,-4764.442,102.360,0,'Horde Eastwall Tower capture - summon Lordaeron Fighter'),
(10692,10,17996,8,2522.425,-4767.049,102.552,0,'Horde Eastwall Tower capture - summon Lordaeron Fighter');

-- set soldiers movement - waypoints need testing
UPDATE `creature_template` SET `MovementType` = 0 WHERE `entry` IN (17647,17996);
UPDATE `creature_template` SET `MovementType` = 2 WHERE `entry` in (17635,17995);
DELETE FROM `creature_movement_template` WHERE `entry` in (17635, 17995);
INSERT INTO `creature_movement_template` (`entry`, `point`, `position_x`, `position_y`, `position_z`) VALUES
(17635, 1, 2501.349, -4725.994, 90.974),
(17635, 2, 2491.211, -4693.162, 82.363),
(17635, 3, 2493.059, -4655.492, 75.194),
(17635, 4, 2562.227, -4646.404, 79.003),
(17635, 5, 2699.748, -4567.377, 87.460),
(17635, 6, 2757.274, -4527.591, 89.080),
(17635, 7, 2850.868, -4417.565, 89.421),
(17635, 8, 2888.340, -4328.486, 90.562),
(17635, 9, 2913.271, -4167.140, 93.919),
(17635, 10, 3035.656, -4260.176, 96.141),
(17635, 11, 3088.544, -4250.208, 97.769),
(17635, 12, 3147.302, -4318.796, 130.410),
(17635, 13, 3166.813, -4349.198, 137.569),

(17995, 1, 2501.349, -4725.994, 90.974),
(17995, 2, 2491.211, -4693.162, 82.363),
(17995, 3, 2493.059, -4655.492, 75.194),
(17995, 4, 2562.227, -4646.404, 79.003),
(17995, 5, 2699.748, -4567.377, 87.460),
(17995, 6, 2757.274, -4527.591, 89.080),
(17995, 7, 2850.868, -4417.565, 89.421),
(17995, 8, 2888.340, -4328.486, 90.562),
(17995, 9, 2913.271, -4167.140, 93.919),
(17995, 10, 3035.656, -4260.176, 96.141),
(17995, 11, 3088.544, -4250.208, 97.769),
(17995, 12, 3147.302, -4318.796, 130.410),
(17995, 13, 3166.813, -4349.198, 137.569);

-- creature linking for EP soldiers
-- INSERT IGNORE INTO creature_linking_template VALUES
-- (17647, 0, 17635, 515, 0),
-- (17996, 0, 17995, 515, 0);
/* ################################# */

-- Halaa fixes
/* ################################# */
-- fire bomb target
UPDATE `creature_template` SET `AIName`='EventAI' WHERE `entry`=18225;
DELETE FROM `creature_ai_scripts` WHERE `creature_id`=18225;
INSERT INTO `creature_ai_scripts` (`id`,`creature_id`,`event_type`,`event_inverse_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action1_type`,`action1_param1`,`action1_param2`,`action1_param3`,`action2_type`,`action2_param1`,`action2_param2`,`action2_param3`,`action3_type`,`action3_param1`,`action3_param2`,`action3_param3`,`comment`) VALUES
('1822501','18225','11','0','100','0','0','0','0','0','11','31961','0','0','20','0','0','0','21','0','0','0','Fire Bomb Target - Cast Fire Bomb on Spawn and set Combat Movement and Auto Attack to false');
-- No random movement for this one - unit flags are guesswork
UPDATE creature_template SET MovementType= 0, unit_flags=unit_flags|33554432 WHERE entry=18225;

-- spawn npcs and gameobjects on the map
DELETE FROM `creature` WHERE `id` IN (18817,18822,21485,21487,21488,18256);
INSERT INTO `creature` (`guid`,`id`,`map`,`spawnMask`,`phaseMask`,`modelid`,`equipment_id`,`position_x`,`position_y`,`position_z`,`orientation`,`spawntimesecs`,`spawndist`,`currentwaypoint`,`curhealth`,`curmana`,`DeathState`,`MovementType`) VALUES
-- alliance
(300009, 18817, 530, 1, 1, 0, 0, -1591.18, 8020.39, -22.2042, 4.59022, 600, 0, 0, 59115, 0, 0, 0),
(300010, 18822, 530, 1, 1, 0, 0, -1588.12, 8019.44, -22.2042, 4.06662, 600, 0, 0, 88710, 0, 0, 0),
(300011, 21485, 530, 1, 1, 0, 0, -1521.93, 7927.37, -20.2299, 3.24631, 600, 0, 0, 104790, 0, 0, 0),
(300012, 21487, 530, 1, 1, 0, 0, -1540.33, 7971.95, -20.7186, 3.07178, 600, 0, 0, 104790, 0, 0, 0),
(300013, 21488, 530, 1, 1, 0, 0, -1570.01, 7993.8, -22.4505, 5.02655, 600, 0, 0, 104790, 0, 0, 0),
-- soldiers
(300014, 18256, 530, 1, 1, 0, 0, -1654.06, 8000.46, -26.59, 3.37, 600, 0, 0, 1182800, 0, 0, 0),
(300015, 18256, 530, 1, 1, 0, 0, -1487.18, 7899.1, -19.53, 0.954, 600, 0, 0, 1182800, 0, 0, 0),
(300016, 18256, 530, 1, 1, 0, 0, -1480.88, 7908.79, -19.19, 4.485, 600, 0, 0, 1182800, 0, 0, 0),
(300017, 18256, 530, 1, 1, 0, 0, -1540.56, 7995.44, -20.45, 0.947, 600, 0, 0, 1182800, 0, 0, 0),
(300018, 18256, 530, 1, 1, 0, 0, -1546.95, 8000.85, -20.72, 6.035, 600, 0, 0, 1182800, 0, 0, 0),
(300019, 18256, 530, 1, 1, 0, 0, -1595.31, 7860.53, -21.51, 3.747, 600, 0, 0, 1182800, 0, 0, 0),
(300020, 18256, 530, 1, 1, 0, 0, -1642.31, 7995.59, -25.8, 3.317, 600, 0, 0, 1182800, 0, 0, 0),
(300021, 18256, 530, 1, 1, 0, 0, -1545.46, 7995.35, -20.63, 1.094, 600, 0, 0, 1182800, 0, 0, 0),
(300022, 18256, 530, 1, 1, 0, 0, -1487.58, 7907.99, -19.27, 5.567, 600, 0, 0, 1182800, 0, 0, 0),
(300023, 18256, 530, 1, 1, 0, 0, -1651.54, 7988.56, -26.52, 2.984, 600, 0, 0, 1182800, 0, 0, 0),
(300024, 18256, 530, 1, 1, 0, 0, -1602.46, 7866.43, -22.11, 4.747, 600, 0, 0, 1182800, 0, 0, 0),
(300025, 18256, 530, 1, 1, 0, 0, -1591.22, 7875.29, -22.35, 4.345, 600, 0, 0, 1182800, 0, 0, 0),
(300026, 18256, 530, 1, 1, 0, 0, -1603.75, 8000.36, -24.18, 4.516, 600, 0, 0, 1182800, 0, 0, 0),
(300027, 18256, 530, 1, 1, 0, 0, -1585.73, 7994.68, -23.29, 4.439, 600, 0, 0, 1182800, 0, 0, 0),
(300028, 18256, 530, 1, 1, 0, 0, -1595.5, 7991.27, -23.53, 4.738, 600, 0, 0, 1182800, 0, 0, 0);
DELETE FROM `creature` WHERE `id` IN (18816,18821,21474,21484,21483,18192);
INSERT INTO `creature` (`guid`,`id`,`map`,`spawnMask`,`phaseMask`,`modelid`,`equipment_id`,`position_x`,`position_y`,`position_z`,`orientation`,`spawntimesecs`,`spawndist`,`currentwaypoint`,`curhealth`,`curmana`,`DeathState`,`MovementType`) VALUES
-- horde
(300029, 18816, 530, 1, 1, 0, 0, -1523.92, 7951.76, -17.6942, 3.51172, 600, 0, 0, 59115, 0, 0, 0),
(300030, 18821, 530, 1, 1, 0, 0, -1527.75, 7952.46, -17.6948, 3.99317, 600, 0, 0, 88710, 0, 0, 0),
(300031, 21474, 530, 1, 1, 0, 0, -1520.14, 7927.11, -20.2527, 3.39389, 600, 0, 0, 104790, 0, 0, 0),
(300032, 21484, 530, 1, 1, 0, 0, -1524.84, 7930.34, -20.182, 3.6405, 600, 0, 0, 104790, 0, 0, 0),
(300033, 21483, 530, 1, 1, 0, 0, -1570.01, 7993.8, -22.4505, 5.02655, 600, 0, 0, 104790, 0, 0, 0),
-- soldiers
(300034, 18192, 530, 1, 1, 0, 0, -1654.06, 8000.46, -26.59, 3.37, 600, 0, 0, 1182800, 0, 0, 0),
(300035, 18192, 530, 1, 1, 0, 0, -1487.18, 7899.1, -19.53, 0.954, 600, 0, 0, 1182800, 0, 0, 0),
(300036, 18192, 530, 1, 1, 0, 0, -1480.88, 7908.79, -19.19, 4.485, 600, 0, 0, 1182800, 0, 0, 0),
(300037, 18192, 530, 1, 1, 0, 0, -1540.56, 7995.44, -20.45, 0.947, 600, 0, 0, 1182800, 0, 0, 0),
(300038, 18192, 530, 1, 1, 0, 0, -1546.95, 8000.85, -20.72, 6.035, 600, 0, 0, 1182800, 0, 0, 0),
(300039, 18192, 530, 1, 1, 0, 0, -1595.31, 7860.53, -21.51, 3.747, 600, 0, 0, 1182800, 0, 0, 0),
(300040, 18192, 530, 1, 1, 0, 0, -1642.31, 7995.59, -25.8, 3.317, 600, 0, 0, 1182800, 0, 0, 0),
(300041, 18192, 530, 1, 1, 0, 0, -1545.46, 7995.35, -20.63, 1.094, 600, 0, 0, 1182800, 0, 0, 0),
(300042, 18192, 530, 1, 1, 0, 0, -1487.58, 7907.99, -19.27, 5.567, 600, 0, 0, 1182800, 0, 0, 0),
(300043, 18192, 530, 1, 1, 0, 0, -1651.54, 7988.56, -26.52, 2.984, 600, 0, 0, 1182800, 0, 0, 0),
(300044, 18192, 530, 1, 1, 0, 0, -1602.46, 7866.43, -22.11, 4.747, 600, 0, 0, 1182800, 0, 0, 0),
(300045, 18192, 530, 1, 1, 0, 0, -1591.22, 7875.29, -22.35, 4.345, 600, 0, 0, 1182800, 0, 0, 0),
(300046, 18192, 530, 1, 1, 0, 0, -1550.6, 7944.45, -21.63, 3.559, 600, 0, 0, 1182800, 0, 0, 0),
(300047, 18192, 530, 1, 1, 0, 0, -1545.57, 7935.83, -21.13, 3.448, 600, 0, 0, 1182800, 0, 0, 0),
(300048, 18192, 530, 1, 1, 0, 0, -1550.86, 7937.56, -21.7, 3.801, 600, 0, 0, 1182800, 0, 0, 0);

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
