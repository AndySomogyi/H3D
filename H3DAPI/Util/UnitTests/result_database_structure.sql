-- --------------------------------------------------------
-- Host:                         127.0.0.1
-- Server version:               10.1.9-MariaDB - mariadb.org binary distribution
-- Server OS:                    Win64
-- HeidiSQL Version:             9.1.0.4867
-- --------------------------------------------------------

/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET NAMES utf8mb4 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;

-- Dumping database structure for testserver
CREATE DATABASE IF NOT EXISTS `testserver` /*!40100 DEFAULT CHARACTER SET utf8 */;
USE `testserver`;


-- Dumping structure for table testserver.performance_results
CREATE TABLE IF NOT EXISTS `performance_results` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `test_run_id` int(10) unsigned NOT NULL,
  `file_id` int(10) unsigned NOT NULL,
  `case_id` int(10) unsigned NOT NULL,
  `min_fps` float unsigned NOT NULL,
  `max_fps` float unsigned NOT NULL,
  `avg_fps` float unsigned NOT NULL,
  `mean_fps` float unsigned NOT NULL,
  `full_case_data` text NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- Data exporting was unselected.


-- Dumping structure for table testserver.screenshot_baselines
CREATE TABLE IF NOT EXISTS `screenshot_baselines` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `test_run_id` int(10) unsigned NOT NULL,
  `file_id` int(10) unsigned NOT NULL,
  `case_id` int(10) unsigned NOT NULL,
  `step_name` tinytext NOT NULL,
  `image` mediumblob NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- Data exporting was unselected.


-- Dumping structure for table testserver.screenshot_results
CREATE TABLE IF NOT EXISTS `screenshot_results` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `test_run_id` int(10) unsigned NOT NULL,
  `file_id` int(10) unsigned NOT NULL,
  `case_id` int(10) unsigned NOT NULL,
  `step_name` tinytext NOT NULL,
  `success` enum('Y','N') NOT NULL,
  `output_image` mediumblob,
  `diff_image` mediumblob,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;

-- Data exporting was unselected.


-- Dumping structure for table testserver.servers
CREATE TABLE IF NOT EXISTS `servers` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `server_name` tinytext NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- Data exporting was unselected.


-- Dumping structure for table testserver.test_cases
CREATE TABLE IF NOT EXISTS `test_cases` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `case_name` text NOT NULL,
  `test_type` enum('performance','rendering') NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- Data exporting was unselected.


-- Dumping structure for table testserver.test_files
CREATE TABLE IF NOT EXISTS `test_files` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `filename` text NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- Data exporting was unselected.


-- Dumping structure for table testserver.test_runs
CREATE TABLE IF NOT EXISTS `test_runs` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `timestamp` datetime NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `server_id` int(10) unsigned NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- Data exporting was unselected.
/*!40101 SET SQL_MODE=IFNULL(@OLD_SQL_MODE, '') */;
/*!40014 SET FOREIGN_KEY_CHECKS=IF(@OLD_FOREIGN_KEY_CHECKS IS NULL, 1, @OLD_FOREIGN_KEY_CHECKS) */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;