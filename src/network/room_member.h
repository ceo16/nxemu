// SPDX-FileCopyrightText: Copyright 2017 Citra Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <functional>
#include <memory>
#include <string>
#include <vector>
#include "yuzu_common/announce_multiplayer_room.h"
#include "yuzu_common/common_types.h"
#include "yuzu_common/socket_types.h"
#include "network/room.h"

namespace Network {

using AnnounceMultiplayerRoom::GameInfo;
using AnnounceMultiplayerRoom::RoomInformation;

enum class LDNPacketType : u8 {
    Scan,
    ScanResp,
    Connect,
    SyncNetwork,
    Disconnect,
    DestroyNetwork,
};

struct LDNPacket {
    LDNPacketType type;
    IPv4Address local_ip;
    IPv4Address remote_ip;
    bool broadcast;
    std::vector<u8> data;
};

/// Information about the received proxy packets.
struct ProxyPacket {
    SockAddrIn local_endpoint;
    SockAddrIn remote_endpoint;
    Protocol protocol;
    bool broadcast;
    std::vector<u8> data;
};

/// Represents a chat message.
struct ChatEntry {
    std::string nickname; ///< Nickname of the client who sent this message.
    /// Web services username of the client who sent this message, can be empty.
    std::string username;
    std::string message; ///< Body of the message.
};

/// Represents a system status message.
struct StatusMessageEntry {
    StatusMessageTypes type; ///< Type of the message
    /// Subject of the message. i.e. the user who is joining/leaving/being banned, etc.
    std::string nickname;
    std::string username;
};

/**
 * This is what a client [person joining a server] would use.
 * It also has to be used if you host a game yourself (You'd create both, a Room and a
 * RoomMembership for yourself)
 */
class RoomMember final {
public:
    enum class State : u8 {
        Uninitialized, ///< Not initialized
        Idle,          ///< Default state (i.e. not connected)
        Joining,       ///< The client is attempting to join a room.
        Joined,    ///< The client is connected to the room and is ready to send/receive packets.
        Moderator, ///< The client is connected to the room and is granted mod permissions.
    };

    enum class Error : u8 {
        // Reasons why connection was closed
        LostConnection, ///< Connection closed
        HostKicked,     ///< Kicked by the host

        // Reasons why connection was rejected
        UnknownError,    ///< Some error [permissions to network device missing or something]
        NameCollision,   ///< Somebody is already using this name
        IpCollision,     ///< Somebody is already using that fake-ip-address
        WrongVersion,    ///< The room version is not the same as for this RoomMember
        WrongPassword,   ///< The password doesn't match the one from the Room
        CouldNotConnect, ///< The room is not responding to a connection attempt
        RoomIsFull,      ///< Room is already at the maximum number of players
        HostBanned,      ///< The user is banned by the host

        // Reasons why moderation request failed
        PermissionDenied, ///< The user does not have mod permissions
        NoSuchUser,       ///< The nickname the user attempts to kick/ban does not exist
    };

    struct MemberInformation {
        std::string nickname;     ///< Nickname of the member.
        std::string username;     ///< The web services username of the member. Can be empty.
        std::string display_name; ///< The web services display name of the member. Can be empty.
        std::string avatar_url;   ///< Url to the member's avatar. Can be empty.
        GameInfo game_info;  ///< Name of the game they're currently playing, or empty if they're
                             /// not playing anything.
        IPv4Address fake_ip; ///< Fake Ip address associated with this member.
    };
    using MemberList = std::vector<MemberInformation>;

    // The handle for the callback functions
    template <typename T>
    using CallbackHandle = std::shared_ptr<std::function<void(const T&)>>;

    /**
     * Unbinds a callback function from the events.
     * @param handle The connection handle to disconnect
     */
    template <typename T>
    void Unbind(CallbackHandle<T> handle);

    RoomMember();
    ~RoomMember();

    /**
     * Returns the status of our connection to the room.
     */
    State GetState() const;

    /**
     * Returns information about the members in the room we're currently connected to.
     */
    const MemberList& GetMemberInformation() const;

    /**
     * Returns the nickname of the RoomMember.
     */
    const std::string& GetNickname() const;

    /**
     * Returns the username of the RoomMember.
     */
    const std::string& GetUsername() const;

    /**
     * Returns the MAC address of the RoomMember.
     */
    const IPv4Address& GetFakeIpAddress() const;

    /**
     * Returns information about the room we're currently connected to.
     */
    RoomInformation GetRoomInformation() const;

    /**
     * Returns whether we're connected to a server or not.
     */
    bool IsConnected() const;

    /**
     * Attempts to join a room at the specified address and port, using the specified nickname.
     */
    void Join(const std::string& nickname, const char* server_addr = "127.0.0.1",
              u16 server_port = DefaultRoomPort, u16 client_port = 0,
              const IPv4Address& preferred_fake_ip = NoPreferredIP,
              const std::string& password = "", const std::string& token = "");

    /**
     * Sends a Proxy packet to the room.
     * @param packet The WiFi packet to send.
     */
    void SendProxyPacket(const ProxyPacket& packet);

    /**
     * Sends an LDN packet to the room.
     * @param packet The WiFi packet to send.
     */
    void SendLdnPacket(const LDNPacket& packet);

    /**
     * Sends a chat message to the room.
     * @param message The contents of the message.
     */
    void SendChatMessage(const std::string& message);

    /**
     * Sends the current game info to the room.
     * @param game_info The game information.
     */
    void SendGameInfo(const GameInfo& game_info);

    /**
     * Sends a moderation request to the room.
     * @param type Moderation request type.
     * @param nickname The subject of the request. (i.e. the user you want to kick/ban)
     */
    void SendModerationRequest(RoomMessageTypes type, const std::string& nickname);

    /**
     * Attempts to retrieve ban list from the room.
     * If success, the ban list callback would be called. Otherwise an error would be emitted.
     */
    void RequestBanList();

    /**
     * Binds a function to an event that will be triggered every time the State of the member
     * changed. The function will be called every time the event is triggered. The callback function
     * must not bind or unbind a function. Doing so will cause a deadlock
     * @param callback The function to call
     * @return A handle used for removing the function from the registered list
     */
    CallbackHandle<State> BindOnStateChanged(std::function<void(const State&)> callback);

    /**
     * Binds a function to an event that will be triggered every time an error happened. The
     * function will be called every time the event is triggered. The callback function must not
     * bind or unbind a function. Doing so will cause a deadlock
     * @param callback The function to call
     * @return A handle used for removing the function from the registered list
     */
    CallbackHandle<Error> BindOnError(std::function<void(const Error&)> callback);

    /**
     * Binds a function to an event that will be triggered every time a ProxyPacket is received.
     * The function will be called every time the event is triggered.
     * The callback function must not bind or unbind a function. Doing so will cause a deadlock
     * @param callback The function to call
     * @return A handle used for removing the function from the registered list
     */
    CallbackHandle<ProxyPacket> BindOnProxyPacketReceived(
        std::function<void(const ProxyPacket&)> callback);

    /**
     * Binds a function to an event that will be triggered every time an LDNPacket is received.
     * The function will be called every time the event is triggered.
     * The callback function must not bind or unbind a function. Doing so will cause a deadlock
     * @param callback The function to call
     * @return A handle used for removing the function from the registered list
     */
    CallbackHandle<LDNPacket> BindOnLdnPacketReceived(
        std::function<void(const LDNPacket&)> callback);

    /**
     * Binds a function to an event that will be triggered every time the RoomInformation changes.
     * The function will be called every time the event is triggered.
     * The callback function must not bind or unbind a function. Doing so will cause a deadlock
     * @param callback The function to call
     * @return A handle used for removing the function from the registered list
     */
    CallbackHandle<RoomInformation> BindOnRoomInformationChanged(
        std::function<void(const RoomInformation&)> callback);

    /**
     * Binds a function to an event that will be triggered every time a ChatMessage is received.
     * The function will be called every time the event is triggered.
     * The callback function must not bind or unbind a function. Doing so will cause a deadlock
     * @param callback The function to call
     * @return A handle used for removing the function from the registered list
     */
    CallbackHandle<ChatEntry> BindOnChatMessageReceived(
        std::function<void(const ChatEntry&)> callback);

    /**
     * Binds a function to an event that will be triggered every time a StatusMessage is
     * received. The function will be called every time the event is triggered. The callback
     * function must not bind or unbind a function. Doing so will cause a deadlock
     * @param callback The function to call
     * @return A handle used for removing the function from the registered list
     */
    CallbackHandle<StatusMessageEntry> BindOnStatusMessageReceived(
        std::function<void(const StatusMessageEntry&)> callback);

    /**
     * Binds a function to an event that will be triggered every time a requested ban list
     * received. The function will be called every time the event is triggered. The callback
     * function must not bind or unbind a function. Doing so will cause a deadlock
     * @param callback The function to call
     * @return A handle used for removing the function from the registered list
     */
    CallbackHandle<Room::BanList> BindOnBanListReceived(
        std::function<void(const Room::BanList&)> callback);

    /**
     * Leaves the current room.
     */
    void Leave();

private:
    class RoomMemberImpl;
    std::unique_ptr<RoomMemberImpl> room_member_impl;
};

inline const char* GetStateStr(const RoomMember::State& s) {
    switch (s) {
    case RoomMember::State::Uninitialized:
        return "Uninitialized";
    case RoomMember::State::Idle:
        return "Idle";
    case RoomMember::State::Joining:
        return "Joining";
    case RoomMember::State::Joined:
        return "Joined";
    case RoomMember::State::Moderator:
        return "Moderator";
    }
    return "Unknown";
}

inline const char* GetErrorStr(const RoomMember::Error& e) {
    switch (e) {
    case RoomMember::Error::LostConnection:
        return "LostConnection";
    case RoomMember::Error::HostKicked:
        return "HostKicked";
    case RoomMember::Error::UnknownError:
        return "UnknownError";
    case RoomMember::Error::NameCollision:
        return "NameCollision";
    case RoomMember::Error::IpCollision:
        return "IpCollision";
    case RoomMember::Error::WrongVersion:
        return "WrongVersion";
    case RoomMember::Error::WrongPassword:
        return "WrongPassword";
    case RoomMember::Error::CouldNotConnect:
        return "CouldNotConnect";
    case RoomMember::Error::RoomIsFull:
        return "RoomIsFull";
    case RoomMember::Error::HostBanned:
        return "HostBanned";
    case RoomMember::Error::PermissionDenied:
        return "PermissionDenied";
    case RoomMember::Error::NoSuchUser:
        return "NoSuchUser";
    default:
        return "Unknown";
    }
}

} // namespace Network
