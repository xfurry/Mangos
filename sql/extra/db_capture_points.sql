-- Missing capture points spawns
/* ################################# */
-- use guids from 500xxx
DELETE FROM `gameobject` WHERE `id` in (181899, 182096, 182097, 182098, 182174, 182173, 182175, 182522, 182523, 182528, 182529);
INSERT INTO `gameobject` (`guid`,`id`,`map`,`spawnMask`,`phaseMask`,`position_x`,`position_y`,`position_z`,`orientation`,`rotation0`,`rotation1`,`rotation2`,`rotation3`,`spawntimesecs`,`animprogress`,`state`) VALUES
-- eastern plaguelads
(500001, 181899, 0, 1, 1, 3181.08, -4379.36, 174.123, -2.03472, -0.065392, 0.119494, -0.842275, 0.521553, 900, 100, 1),
(500002, 182096, 0, 1, 1, 1860.85, -3731.23, 196.716, -2.53214, 0.033967, -0.131914, 0.944741, -0.298177, 180, 100, 1),
(500003, 182097, 0, 1, 1, 2574.51, -4794.89, 144.704, -1.45003, -0.097056, 0.095578, -0.656229, 0.742165, 180, 100, 1),
(500004, 182098, 0, 1, 1, 2962.71, -3042.31, 154.789, 2.08426, -0.074807, -0.113837, 0.855928, 0.49883, 900, 100, 1),
-- hellfire
(500005, 182174, 530, 1, 1, -184.889, 3476.93, 38.205, -0.017453, 0, 0, -0.00872639, 0.999962, 180, 100, 1),
(500006, 182173, 530, 1, 1, -290.016, 3702.42, 56.6729, 0.034907, 0, 0, 0.0174526, 0.999848, 180, 100, 1),
(500007, 182175, 530, 1, 1, -471.462, 3451.09, 34.6432, 0.174533, 0, 0, 0.0871558, 0.996195, 180, 100, 1),
-- zangamarsh
(500008, 182522, 530, 1, 1, 336.466, 7340.26, 41.4984, -1.58825, 0, 0, -0.713251, 0.700909, 180, 100, 1),
(500009, 182523, 530, 1, 1, 303.243, 6841.36, 40.1245, -1.58825, 0, 0, -0.713251, 0.700909, 180, 100, 1),
(500010, 182528, 530, 1, 1, 253.535, 7083.8, 36.9946, -0.0174535, 0, 0, -0.00872664, 0.999962, -300, 100, 1),
(500011, 182529, 530, 1, 1, 253.535, 7083.8, 36.9946, -0.0174535, 0, 0, -0.00872664, 0.999962, 300, 100, 1),
-- terokkar
(500012, 183104, 530, 1, 1, -3081.65, 5335.03, 17.1853, -2.14675, 0, 0, -0.878816, 0.477161, 180, 100, 1),
(500013, 183411, 530, 1, 1, -2939.9, 4788.73, 18.987, 2.77507, 0, 0, 0.983255, 0.182237, 180, 100, 1),
(500014, 183412, 530, 1, 1, -3174.94, 4440.97, 16.2281, 1.8675, 0, 0, 0.803856, 0.594824, 180, 100, 1),
(500015, 183413, 530, 1, 1, -3603.31, 4529.15, 20.9077, 0.994838, 0, 0, 0.477159, 0.878817, 180, 100, 1),
(500016, 183414, 530, 1, 1, -3812.37, 4899.3, 17.7249, 0.087266, 0, 0, 0.0436192, 0.999048, 180, 100, 1);
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
-- the red and blue lights need some updates - from ytdb
DELETE FROM `creature` WHERE `id` IN (18759,18757);
INSERT INTO `creature` (`guid`,`id`,`map`,`spawnMask`,`phaseMask`,`modelid`,`equipment_id`,`position_x`,`position_y`,`position_z`,`orientation`,`spawntimesecs`,`spawndist`,`currentwaypoint`,`curhealth`,`curmana`,`DeathState`,`MovementType`) VALUES
(300003, 18759, 530, 1, 1, 0, 0, 273.866, 7082.68, 87.0635, 3.01942, 180, 0, 0, 1, 0, 0, 0),
(300004, 18759, 530, 1, 1, 0, 0, 375.806, 7332.96, 63.1065, 2.94961, 180, 0, 0, 1, 0, 0, 0),
(300005, 18759, 530, 1, 1, 0, 0, 340.453, 6833.1, 61.798, 3.01942, 180, 0, 0, 1, 0, 0, 0),
(300006, 18757, 530, 1, 1, 0, 0, 273.866, 7082.68, 87.0635, 3.01942, 180, 0, 0, 1, 0, 0, 0),
(300007, 18757, 530, 1, 1, 0, 0, 375.806, 7332.96, 63.1065, 2.94961, 180, 0, 0, 1, 0, 0, 0),
(300008, 18757, 530, 1, 1, 0, 0, 340.453, 6833.1, 61.798, 3.01942, 180, 0, 0, 1, 0, 0, 0);
UPDATE `creature` SET `id` = 18757, `position_x` = 340.453, `position_y` = 6833.1, `position_z` = 61.798 WHERE `guid` = 67037;
UPDATE `creature` SET `MovementType` = 0, `spawndist` = 0 WHERE `id` IN (18757,18759);
-- negative spawntimes for Zanga banner
UPDATE `gameobject` SET `spawntimesecs` = -181 WHERE `id` = 182527;
-- red aura in c_t_a
DELETE FROM `creature_template_addon` WHERE (`entry`=18757);
INSERT INTO `creature_template_addon` (`entry`, `mount`, `bytes1`, `b2_0_sheath`, `b2_1_pvp_state`, `emote`, `moveflags`, `auras`) VALUES (18757, 0, 0, 1, 0, 0, 0, 32839);
/* ################################# */


-- Zangarmarsh gossip - may not be 100% blizzlike
/* ################################# */
-- Set custom menu id for the horde
Update creature_template set gossip_menu_id = 7723 where entry = 18564;
-- links the gossip menu with the npc text for the horde npc
delete from gossip_menu where entry in (7723);
insert into gossip_menu (entry, text_id) values
(7723,9431);
-- Alliance & horde - stores the text and the script id
delete from gossip_menu_option where menu_id in (30001,30002);
insert into gossip_menu_option (menu_id, id, option_icon, option_text, option_id, npc_option_npcflag, action_script_id) values
(7724,0,0,'[PH] Zangarmarsh PvP Banner',1,1,3000001),
(7723,1,1,'I have marks to redeem!',3,128,0),
(7723,0,0,'[PH] Zangarmarsh PvP Banner',1,1,3000002);
-- Alliance & horde - stores the gossip script
delete from gossip_scripts where id in (3000001,3000002);
insert into gossip_scripts(id, delay, command, datalong, datalong2, comments) values
(3000001,0,15,32430,0,'Cast Battle Standard - Alliance'),
(3000002,0,15,32431,0,'Cast Battle Standard - Horde');
/* ################################# */


-- Eastern Plaguelands fixes
/* ################################# */
-- set shrine respawn
UPDATE `gameobject` SET `spawntimesecs` = -180 WHERE `id` in (181682, 181955);
-- correct horde faction
UPDATE `gameobject_template` SET `faction` = 1314 WHERE `entry` = 181955;
-- set flightmaster movement to idle
UPDATE `creature_template` SET `MovementType` = 0 WHERE `entry` = 17209;

-- set soldiers movement
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
 INSERT IGNORE INTO creature_linking_template VALUES
 (17647, 0, 17635, 515),
 (17996, 0, 17995, 515);
 /* ################################# */
