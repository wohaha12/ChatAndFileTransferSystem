-- 创建表结构
USE chat_file_transfer;

-- 用户表
CREATE TABLE IF NOT EXISTS t_user (
    id BIGINT UNSIGNED NOT NULL AUTO_INCREMENT COMMENT '用户ID',
    username VARCHAR(64) NOT NULL COMMENT '用户名',
    password_hash VARCHAR(128) NOT NULL COMMENT '密码哈希',
    salt VARCHAR(32) NOT NULL COMMENT '密码盐值',
    nickname VARCHAR(64) DEFAULT '' COMMENT '昵称',
    avatar_url VARCHAR(255) DEFAULT '' COMMENT '头像URL',
    status TINYINT NOT NULL DEFAULT 0 COMMENT '状态：0-离线，1-在线，2-隐身',
    create_time DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP COMMENT '创建时间',
    update_time DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP COMMENT '更新时间',
    PRIMARY KEY (id),
    UNIQUE KEY uk_username (username),
    KEY idx_status (status)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COMMENT='用户表';

-- 逻辑文件表
CREATE TABLE IF NOT EXISTS t_file_info (
    id BIGINT UNSIGNED NOT NULL AUTO_INCREMENT COMMENT '文件ID',
    user_id BIGINT UNSIGNED NOT NULL COMMENT '用户ID',
    file_name VARCHAR(255) NOT NULL COMMENT '文件名',
    file_path VARCHAR(512) NOT NULL COMMENT '文件路径(相对路径)',
    file_size BIGINT UNSIGNED NOT NULL DEFAULT 0 COMMENT '文件大小(字节)',
    file_hash VARCHAR(64) DEFAULT '' COMMENT '文件哈希值(MD5)',
    file_type TINYINT NOT NULL DEFAULT 0 COMMENT '文件类型：0-文件，1-目录',
    parent_id BIGINT UNSIGNED DEFAULT 0 COMMENT '父目录ID，0表示根目录',
    is_deleted TINYINT NOT NULL DEFAULT 0 COMMENT '是否已删除：0-未删除，1-已删除',
    create_time DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP COMMENT '创建时间',
    update_time DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP COMMENT '更新时间',
    PRIMARY KEY (id),
    KEY idx_user_id (user_id),
    KEY idx_parent_id (parent_id),
    KEY idx_file_hash (file_hash),
    KEY idx_is_deleted (is_deleted),
    FOREIGN KEY (user_id) REFERENCES t_user (id) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COMMENT='逻辑文件表';

-- 物理文件表
CREATE TABLE IF NOT EXISTS t_file_store (
    id BIGINT UNSIGNED NOT NULL AUTO_INCREMENT COMMENT '物理文件ID',
    file_hash VARCHAR(64) NOT NULL COMMENT '文件哈希值(MD5)',
    file_size BIGINT UNSIGNED NOT NULL COMMENT '文件大小(字节)',
    storage_path VARCHAR(512) NOT NULL COMMENT '物理存储路径',
    storage_server_id INT UNSIGNED NOT NULL COMMENT '存储服务器ID',
    ref_count INT UNSIGNED NOT NULL DEFAULT 1 COMMENT '引用计数',
    first_upload_time DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP COMMENT '首次上传时间',
    last_access_time DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP COMMENT '最后访问时间',
    PRIMARY KEY (id),
    UNIQUE KEY uk_file_hash (file_hash),
    KEY idx_storage_server (storage_server_id),
    KEY idx_ref_count (ref_count)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COMMENT='物理文件表';

-- 存储服务器表
CREATE TABLE IF NOT EXISTS t_storage_server (
    id INT UNSIGNED NOT NULL AUTO_INCREMENT COMMENT '服务器ID',
    server_name VARCHAR(64) NOT NULL COMMENT '服务器名称',
    ip_address VARCHAR(45) NOT NULL COMMENT 'IP地址',
    port SMALLINT UNSIGNED NOT NULL COMMENT '端口',
    status TINYINT NOT NULL DEFAULT 0 COMMENT '状态：0-离线，1-在线，2-维护中',
    capacity_total BIGINT UNSIGNED NOT NULL DEFAULT 0 COMMENT '总容量(字节)',
    capacity_used BIGINT UNSIGNED NOT NULL DEFAULT 0 COMMENT '已用容量(字节)',
    weight INT UNSIGNED NOT NULL DEFAULT 1 COMMENT '负载均衡权重',
    create_time DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP COMMENT '创建时间',
    update_time DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP COMMENT '更新时间',
    PRIMARY KEY (id),
    UNIQUE KEY uk_ip_port (ip_address, port),
    KEY idx_status (status)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COMMENT='存储服务器表';

-- 好友关系表
CREATE TABLE IF NOT EXISTS t_friend (
    id BIGINT UNSIGNED NOT NULL AUTO_INCREMENT COMMENT '关系ID',
    user_id BIGINT UNSIGNED NOT NULL COMMENT '用户ID',
    friend_id BIGINT UNSIGNED NOT NULL COMMENT '好友ID',
    create_time DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP COMMENT '创建时间',
    update_time DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP COMMENT '更新时间',
    PRIMARY KEY (id),
    UNIQUE KEY uk_user_friend (user_id, friend_id),
    KEY idx_user_id (user_id),
    KEY idx_friend_id (friend_id),
    FOREIGN KEY (user_id) REFERENCES t_user (id) ON DELETE CASCADE,
    FOREIGN KEY (friend_id) REFERENCES t_user (id) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COMMENT='好友关系表';

-- 聊天消息表
CREATE TABLE IF NOT EXISTS t_chat_message (
    id BIGINT UNSIGNED NOT NULL AUTO_INCREMENT COMMENT '消息ID',
    sender_id BIGINT UNSIGNED NOT NULL COMMENT '发送者ID',
    receiver_id BIGINT UNSIGNED NOT NULL COMMENT '接收者ID',
    message_type TINYINT NOT NULL DEFAULT 0 COMMENT '消息类型：0-文本，1-文件，2-图片',
    content TEXT COMMENT '消息内容',
    file_id BIGINT UNSIGNED DEFAULT NULL COMMENT '文件ID(如果是文件消息)',
    create_time DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP COMMENT '创建时间',
    PRIMARY KEY (id),
    KEY idx_sender_id (sender_id),
    KEY idx_receiver_id (receiver_id),
    KEY idx_create_time (create_time),
    FOREIGN KEY (sender_id) REFERENCES t_user (id) ON DELETE CASCADE,
    FOREIGN KEY (receiver_id) REFERENCES t_user (id) ON DELETE CASCADE,
    FOREIGN KEY (file_id) REFERENCES t_file_info (id) ON DELETE SET NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COMMENT='聊天消息表';

-- 显示表创建成功信息
SELECT 'All tables created successfully!' AS Message;
