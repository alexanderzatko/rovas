<?php

/**
* Implements hook_default_rules_configuration().
*/
function rovas_import_rule_default_rules_configuration() {
$items = array();
$items['rules_verification_reminder'] = entity_import('rules_config', '{ "rules_verification_reminder" : {
    "LABEL" : "verification reminder",
    "DESCRIPTION" : "scans all outstanding verification slots and sends emails to their owners to verify the reports",
    "PLUGIN" : "action set",
    "OWNER" : "rules",
    "REQUIRES" : [
      "views_bulk_operations",
      "rules",
      "php",
      "rules_i18n",
      "rules_conditional"
    ],
    "ACTION SET" : [
      { "views_bulk_operations_action_load_list" : {
          "USING" : { "view" : "pending_verifications|default" },
          "PROVIDE" : { "entity_list" : { "slots" : "slots" } }
        }
      },
      { "variable_add" : {
          "USING" : { "type" : "user" },
          "PROVIDE" : { "variable_added" : { "user" : "user" } }
        }
      },
      { "variable_add" : {
          "USING" : { "type" : "text" },
          "PROVIDE" : { "variable_added" : { "message" : "message" } }
        }
      },
      { "LOOP" : {
          "USING" : { "list" : [ "slots" ] },
          "ITEM" : { "slot" : "slot" },
          "DO" : [
            { "variable_add" : {
                "USING" : {
                  "type" : "text",
                  "value" : "\u003C?php\r\n\r\n$object = entity_metadata_wrapper(\u0027node\u0027, $slot-\u003Enid);\r\n$ex_url = $object-\u003Efield_report_to_verify-\u003Efield_web_address-\u003Evalue();\r\n$rep_id = $object-\u003Efield_report_to_verify-\u003EgetIdentifier();\r\n$rep_name = $object-\u003Efield_report_to_verify-\u003Elabel();\r\n\r\necho \u0027\u003Cstrong\u003E\u0027;\r\necho $rep_name;\r\necho \u0027\u003C\/strong\u003E\u0027;\r\necho \u0027 deadline: \u0027;\r\necho date(\u0027d M Y i:m:s\u0027,$object-\u003Efield_verify_date-\u003Evalue());\r\necho \u0027\u003Cbr\u003E\u0027;\r\nif (!empty($ex_url)) {\r\n\t$parsed = parse_url($ex_url);\r\n\t$domain = $parsed[\u0027host\u0027];\r\n\techo \u0027\u003Ca href=\u0022\u0027.$ex_url.\u0027\u0022\u003ESee in \u0027 . $domain . \u0027\u003C\/a\u003E\u0027;\r\n\techo \u0027\u0026nbsp;\u0026nbsp;\u0027;\r\n}\r\necho \u0027\u003Ca href=\u0022\/node\/\u0027.$rep_id.\u0027\u0022\u003EVerify in Rovas\u003C\/a\u003E\u0027;\r\n?\u003E"
                },
                "PROVIDE" : { "variable_added" : { "msg_core" : "msg core" } }
              }
            },
            { "CONDITIONAL" : [
                {
                  "IF" : { "data_is" : { "data" : [ "user" ], "value" : [ "slot:author" ] } },
                  "DO" : [
                    { "data_set" : {
                        "data" : [ "message" ],
                        "value" : "\u003C?php\r\n\r\nif (!empty($message)) {\r\n\techo $message;\r\n\techo \u0027\u003Cbr\u003E\u003Cbr\u003E\u0027;\r\n}\r\n\r\necho $msg_core;\r\n?\u003E"
                      }
                    }
                  ]
                },
                {
                  "ELSE IF" : { "data_is_empty" : { "data" : [ "user" ] } },
                  "DO" : [
                    { "data_set" : {
                        "data" : [ "message" ],
                        "value" : "\u003C?php\r\n\r\necho $msg_core;\r\n?\u003E"
                      }
                    }
                  ]
                },
                { "ELSE" : [
                    { "CONDITIONAL" : [
                        {
                          "IF" : { "data_is" : { "data" : [ "slot:author:field-verification-reminders" ], "value" : "1" } },
                          "DO" : [
                            { "CONDITIONAL" : [
                                {
                                  "IF" : { "php_eval" : { "code" : "return isset($user-\u003Edata[\u0027hybridauth\u0027][\u0027provider\u0027]) \u0026\u0026 $user-\u003Edata[\u0027hybridauth\u0027][\u0027provider\u0027] == \u0027Nabezky\u0027;" } },
                                  "DO" : [
                                    { "variable_add" : {
                                        "USING" : {
                                          "type" : "text",
                                          "value" : "To log into Rovas, click the Na bezky icon \u003Cimg src=\u0022\/sites\/all\/modules\/rovas_ha\/plugins\/icon_pack\/hybridauth_32\/icons\/nabezky.svg\u0022 \/\u003E you can find in the login block."
                                        },
                                        "PROVIDE" : { "variable_added" : { "login_with_nabezky" : "login with nabezky" } }
                                      }
                                    }
                                  ]
                                },
                                { "ELSE" : [
                                    { "variable_add" : {
                                        "USING" : { "type" : "text" },
                                        "PROVIDE" : { "variable_added" : { "login_with_nabezky" : "login with nabezky" } }
                                      }
                                    }
                                  ]
                                }
                              ]
                            },
                            { "mail" : {
                                "to" : "sano@strings.net",
                                "subject" : "Rovas - verification reminder [user:name]",
                                "message" : "This is a reminder, that you have not verified yet the below-listed reports.\r\n\r\n[message:value]\r\n\r\n[login-with-nabezky:value]\r\nYou can cancel these notifications in \u003Ca href=\u0022[user:edit-url]\u0022\u003Eyour user profile\u003C\/a\u003E, but by ignoring the verification requests you will lose the ability to earn chrons.",
                                "language" : [ "slot:author:language" ]
                              }
                            }
                          ]
                        }
                      ]
                    },
                    { "data_set" : { "data" : [ "message" ], "value" : "[msg-core:value]" } }
                  ]
                }
              ]
            },
            { "data_set" : { "data" : [ "user" ], "value" : [ "slot:author" ] } }
          ]
        }
      }
    ]
  }
}');
return $items;
}
