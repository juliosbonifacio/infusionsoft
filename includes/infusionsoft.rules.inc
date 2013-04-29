<?php
/**
 * @file
 * Implementation of rules for infusionsoft
 * http://drupal.org/project/rules
 */

/**
 * Implements hook_rules_action_info().
 * @ingroup rules
 *
 * Defining infusionsoft add to contact group action.
 */
function infusionsoft_rules_condition_info() {
  return array(
    'infusionsoft_condition_user_has_tag' => array(
      'label' => t('User has an infusionsoft tag'),
      'module' => 'Infusionsoft',
      'arguments' => array(
        'user' => array('type' => 'user', 'label' => t('User')),
      ),
      'module' => 'User',
    ),
  );
}

/**
 * Condition Implementation: Condition that a user has a specific infusionsoft tag
 * @ingroup rules
 */
function infusionsoft_condition_user_has_tag($user, $settings) {
  if ($user->uid > 0) {
    $contact_id = infusionsoft_contactid_by_user($user);
    $query = array(
      'UserId' => $contact_id,
      'GroupId' => $settings['tag'],
    );
    $return_fields = array('UserId');
    $result = infusionsoft_send_request('DataService.query', 'GroupAssign', 1, 0, $query, $return_fields);
    if (empty($result) != TRUE) return TRUE;
  }
  return FALSE;
}

/**
 *  Implementation of Drupal rules condition form.
 * @ingroup rules
 *  Reference: http://drupal.org/node/298534
 *
 * A drupal form which allows you to select an infusionsoft tag
 *
 */
function infusionsoft_condition_user_has_tag_form($settings, &$form) {
  $form['settings']['tag'] = array(
    '#title' => t('Tags'),
    '#type' => 'select',
    '#description' => t('Select a tag you would like to add to a user'),
    '#options' => _infusionsoft_get_contact_group_select(),
    '#default_value' => isset($settings['tag']) ? $settings['tag'] : '',
  );
  $form['#validate'][] = 'infusionsoft_condition_user_has_tag_validate';
  $form['#submit'][] = 'infusionsoft_condition_user_has_tag_submit';
}

function infusionsoft_condition_user_has_tag_validate($form, $form_state) {
  $numeric = is_numeric($form['settings']['tag']);
  if (!$numeric) form_set_error('tag', 'invalid option');
}

function infusionsoft_condition_user_has_tag_submit(&$settings, $form, $form_state) {
  $settings['tag'] = check_plain($form['settings']['tag']);
}