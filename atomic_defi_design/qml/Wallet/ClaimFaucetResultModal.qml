import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15

import "../Components"
import "../Constants"
import App 1.0

BasicModal {
    readonly property var claiming_faucet_rpc_result: api_wallet_page.claiming_faucet_rpc_data

    id: root
    width: 1200

    ModalContent {
        id: status

        title: claiming_faucet_rpc_result && claiming_faucet_rpc_result.status ?
                   claiming_faucet_rpc_result.status : ""

        DefaultText {
            id: message

            text_value: claiming_faucet_rpc_result && claiming_faucet_rpc_result.message ?
                            claiming_faucet_rpc_result.message : ""
        }
    }
}
