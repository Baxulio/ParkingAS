-- --------------------------------------------------------
-- Хост:                         localhost
-- Версия сервера:               5.7.16 - MySQL Community Server (GPL)
-- ОС Сервера:                   Win32
-- HeidiSQL Версия:              9.3.0.4984
-- --------------------------------------------------------

/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET NAMES utf8mb4 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;

-- Дамп структуры базы данных Parking
CREATE DATABASE IF NOT EXISTS `Parking` /*!40100 DEFAULT CHARACTER SET utf8 */;
USE `Parking`;


-- Дамп структуры для таблица Parking.Active
CREATE TABLE IF NOT EXISTS `Active` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `rf_id` int(11) unsigned NOT NULL,
  `in_time` datetime NOT NULL,
  `in_number` tinyint(3) unsigned NOT NULL,
  `img` tinytext NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- Экспортируемые данные не выделены.


-- Дамп структуры для таблица Parking.Cards
CREATE TABLE IF NOT EXISTS `Cards` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `priceId` int(11) NOT NULL,
  `code` int(11) NOT NULL,
  `subscription` tinyint(1) NOT NULL DEFAULT '0',
  PRIMARY KEY (`id`),
  KEY `FK_Cards_Price` (`priceId`),
  CONSTRAINT `FK_Cards_Price` FOREIGN KEY (`priceId`) REFERENCES `Price` (`id`) ON DELETE CASCADE ON UPDATE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- Экспортируемые данные не выделены.


-- Дамп структуры для таблица Parking.History
CREATE TABLE IF NOT EXISTS `History` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `rf_id` int(11) unsigned NOT NULL,
  `in_time` datetime NOT NULL,
  `out_time` datetime DEFAULT NULL,
  `in_number` tinyint(3) unsigned NOT NULL,
  `out_number` tinyint(3) unsigned NOT NULL,
  `price` decimal(10,2) NOT NULL,
  `img` tinytext NOT NULL,
  `img_out` tinytext NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- Экспортируемые данные не выделены.


-- Дамп структуры для процедура Parking.MoveToHistory
DELIMITER //
CREATE DEFINER=`root`@`%` PROCEDURE `MoveToHistory`(IN `b_rf_id` INT, IN `b_out_number` TINYINT, IN `b_img_out` TINYTEXT)
    MODIFIES SQL DATA
BEGIN

DECLARE b_id INT;
DECLARE b_in_time DATETIME;
DECLARE b_in_number TINYINT;
DECLARE b_img TINYTEXT;

DECLARE b_price DOUBLE;

DECLARE b_last_insert_id INT;

SELECT  `id`, `in_time`, `in_number`, `img` 
FROM `Parking`.`Active` 
WHERE b_rf_id=`rf_id` LIMIT 1
INTO b_id, b_in_time, b_in_number, b_img;

SELECT `price_per_hour` 
FROM `Parking`.`Price` 
INTO b_price;

INSERT INTO `Parking`.`History` (`rf_id`, `in_time`, `out_time`, `in_number`, `out_number`, `price`, `img`, `img_out`) 
VALUES (b_rf_id, b_in_time, SYSDATE(), b_in_number, b_out_number, TIMESTAMPDIFF(MINUTE,b_in_time,SYSDATE())*b_price/60, b_img, b_img_out);

SET b_last_insert_id = LAST_INSERT_ID();

DELETE FROM `Parking`.`Active` 
WHERE  `id`=b_id;

SELECT `in_time`, `out_time`, `in_number`, `price` 
FROM `Parking`.`History` 
WHERE `id` = b_last_insert_id;

END//
DELIMITER ;


-- Дамп структуры для таблица Parking.Price
CREATE TABLE IF NOT EXISTS `Price` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `car_type` tinytext NOT NULL,
  `price_formula` tinytext NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- Экспортируемые данные не выделены.


-- Дамп структуры для процедура Parking.RegisterCar
DELIMITER //
CREATE DEFINER=`root`@`%` PROCEDURE `RegisterCar`(IN `b_rfid` INT, IN `b_in_number` TINYINT, IN `b_img` TINYTEXT)
    MODIFIES SQL DATA
BEGIN

INSERT INTO `Parking`.`Active` (`rf_id`, `in_time`, `in_number`, `img`) 
VALUES (b_rfid,SYSDATE(), b_in_number, b_img);

END//
DELIMITER ;
/*!40101 SET SQL_MODE=IFNULL(@OLD_SQL_MODE, '') */;
/*!40014 SET FOREIGN_KEY_CHECKS=IF(@OLD_FOREIGN_KEY_CHECKS IS NULL, 1, @OLD_FOREIGN_KEY_CHECKS) */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
